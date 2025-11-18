#!/bin/bash
# Download and test latest Windows build from GitHub Actions

set -e

OWNER="GustavoLR548"
REPO="fiend"

echo "=== Fetching latest Windows build from GitHub Actions ==="

# Get the latest successful workflow run
echo "Finding latest successful workflow run..."
WORKFLOW_RUN=$(gh api \
  "/repos/$OWNER/$REPO/actions/runs?status=success&per_page=1" \
  --jq '.workflow_runs[0]')

RUN_ID=$(echo "$WORKFLOW_RUN" | jq -r '.id')
RUN_NUMBER=$(echo "$WORKFLOW_RUN" | jq -r '.run_number')
echo "Found run #$RUN_NUMBER (ID: $RUN_ID)"

# Download the Windows artifact
echo "Downloading Windows artifact..."
ARTIFACT_ID=$(gh api "/repos/$OWNER/$REPO/actions/runs/$RUN_ID/artifacts" \
  --jq '.artifacts[] | select(.name == "fiend-windows-x64-bundled") | .id')

if [ -z "$ARTIFACT_ID" ]; then
    echo "ERROR: No Windows artifact found!"
    exit 1
fi

DOWNLOAD_DIR="test-windows-build"
rm -rf "$DOWNLOAD_DIR"
mkdir -p "$DOWNLOAD_DIR"

gh api "/repos/$OWNER/$REPO/actions/artifacts/$ARTIFACT_ID/zip" > "$DOWNLOAD_DIR/artifact.zip"
cd "$DOWNLOAD_DIR"
unzip -q artifact.zip
echo "Extracted to $DOWNLOAD_DIR/"

# Run with Wine if available
if command -v wine &> /dev/null; then
    echo ""
    echo "=== Testing with Wine ==="
    echo "Press Ctrl+C to stop"
    echo ""
    wine fiend.exe --log-level debug
else
    echo ""
    echo "Wine not installed. To test, run:"
    echo "  cd $DOWNLOAD_DIR && wine fiend.exe"
fi
