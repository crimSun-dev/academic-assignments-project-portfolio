# Run OWASP dependency-check for Module2.2 (use X: drive to avoid path issues)
$ErrorActionPreference = "Stop"

$projectDrive = "X:\"
if (-not (Test-Path "$projectDrive\pom.xml")) {
    Write-Host "X:\ is not mapped to Module2.2. Map it first with:"
    Write-Host '  subst X: "G:\Draven''s Very Important Files\Workspace\SNHU Courses\Term 2\CS-305-14156-M01 Software Security 2026 C-5 (Aug - Oct) - 8312026 - 1024 AM\Module 02\MODULE TWO CODING ASSIGNMENT\Module2.2"'
    exit 1
}

Set-Location $projectDrive
$env:JAVA_HOME = "C:\Program Files\Java\jdk-24"
$java = Join-Path $env:JAVA_HOME "bin\java.exe"

# Remove stale lock if no other dependency-check/java scan is running
$lockFile = Join-Path $env:USERPROFILE ".m2\repository\org\owasp\dependency-check-data\11.0\odc.update.lock"
if (Test-Path $lockFile) {
    $javaProcs = Get-Process java -ErrorAction SilentlyContinue
    if (-not $javaProcs) {
        Remove-Item $lockFile -Force
        Write-Host "Removed stale NVD update lock."
    } else {
        Write-Host "Java is running — close Eclipse/other scans first, then retry."
        exit 1
    }
}

Write-Host "Starting dependency-check (first run may take 10-20 minutes)..."
& $java -classpath .mvn\wrapper\maven-wrapper.jar `
    "-Dmaven.multiModuleProjectDirectory=$projectDrive" `
    org.apache.maven.wrapper.MavenWrapperMain `
    dependency-check:check

if ($LASTEXITCODE -ne 0) {
    Write-Host "Scan failed. If you see a lock error, close Eclipse and run this script again."
    exit $LASTEXITCODE
}

$report = Join-Path $projectDrive "target\dependency-check-report.html"
if (Test-Path $report) {
    Write-Host "Opening report: $report"
    Start-Process $report
} else {
    Write-Host "Report not found at $report"
    exit 1
}
