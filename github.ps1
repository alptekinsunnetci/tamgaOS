param (
    [string]$RepoPath = "C:\Users\Administrator\Desktop\netio.monitor",
    [string]$Branch = "main"
)

$ErrorActionPreference = "Stop"

$versionFile = Join-Path $RepoPath ".version"

Set-Location $RepoPath

Write-Host "== Repo sync ==" -ForegroundColor Cyan

# Pull
git pull --rebase origin $Branch

# Version check
if (!(Test-Path $versionFile)) {
    throw ".version dosyası bulunamadı!"
}

$currentVersion = (Get-Content $versionFile -Raw).Trim()

if ($currentVersion -notmatch "^v(\d+)\.(\d+)\.(\d+)$") {
    throw "Versiyon formatı hatalı: $currentVersion"
}

# Regex capture fix
$major = [int]$matches[1]
$minor = [int]$matches[2]
$patch = [int]$matches[3]

# PATCH bump
$patch++

# rollover logic
if ($patch -ge 10) {
    $patch = 0
    $minor++
}

$newVersion = "v$major.$minor.$patch"

Write-Host "Eski: $currentVersion"
Write-Host "Yeni: $newVersion"

# atomic write
Set-Content -Path $versionFile -Value $newVersion -NoNewline

# check git changes
$changes = git status --porcelain
if (-not $changes) {
    Write-Host "Değişiklik yok, commit atlanıyor." -ForegroundColor Yellow
    exit 0
}

git add .version
git commit -m "release: $currentVersion -> $newVersion"

# annotated tag (best practice)
git tag -a $newVersion -m "Release $newVersion"

Write-Host "== Push ==" -ForegroundColor Cyan

git push origin $Branch
git push origin $newVersion

Write-Host "DONE: $newVersion" -ForegroundColor Green