#!/usr/bin/env bash
set -eu

LAB_DIR="$(mktemp -d "${TMPDIR:-/tmp}/git-staging.XXXXXX")"
REPO="$LAB_DIR/repo"
git init -q -b main "$REPO"
git -C "$REPO" config user.name "Git Study"
git -C "$REPO" config user.email "git-study@example.invalid"

printf 'version 1\n' > "$REPO/note.txt"
git -C "$REPO" add note.txt
git -C "$REPO" commit -q -m "initial note"

printf 'version 2\n' > "$REPO/note.txt"
git -C "$REPO" add note.txt
printf 'version 2\nworking tree only\n' > "$REPO/note.txt"

printf '%s\n' '--- staged and unstaged changes together ---'
git -C "$REPO" status --short
printf '%s\n' '--- staged snapshot ---'
git -C "$REPO" show :note.txt
printf '%s\n' '--- working tree file ---'
git -C "$REPO" cat-file -p :note.txt >/dev/null
sed -n '1,10p' "$REPO/note.txt"

git -C "$REPO" restore --staged note.txt
printf '%s\n' '--- after restore --staged ---'
git -C "$REPO" status --short

git -C "$REPO" add note.txt
git -C "$REPO" commit -q -m "update note"
git -C "$REPO" rm --cached note.txt >/dev/null
printf '%s\n' '--- after git rm --cached ---'
git -C "$REPO" status --short
test -f "$REPO/note.txt"
printf 'working-tree file remains: %s\n' "$REPO/note.txt"
printf 'lab kept at: %s\n' "$LAB_DIR"
