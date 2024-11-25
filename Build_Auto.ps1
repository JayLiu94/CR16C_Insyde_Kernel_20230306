# 確認 BUILD.BAT 存在
if (-Not (Test-Path -Path "./OEM/PROJECT/BUILD.BAT")) {
    Write-Host "ERROR: BUILD.BAT not found in OEM\PROJECT" -ForegroundColor Red
    exit 1
}

# 儲存當前路徑
$currentPath = Get-Location

# 切換到目標資料夾
Set-Location -Path "./OEM/PROJECT"

Write-Host "BUILD.BAT ready to execute ..." -ForegroundColor Blue
# 執行 BUILD.BAT 並檢查返回碼
& .\BUILD.BAT
if ($LASTEXITCODE -ne 0) {
    Write-Host "BUILD.BAT failed with error code $LASTEXITCODE." -ForegroundColor Red
    # 切回初始目錄
    Set-Location -Path $currentPath
    exit $LASTEXITCODE
}

# 切回初始目錄
Set-Location -Path $currentPath

# 成功訊息 (綠色)
Write-Host "BUILD.BAT executed complemently." -ForegroundColor Green
