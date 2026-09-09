#!/usr/bin/env bash
set -eu

LAB_DIR="$(mktemp -d "${TMPDIR:-/tmp}/git-branches.XXXXXX")"
REPO="$LAB_DIR/repo"
git init -q -b main "$REPO"
git -C "$REPO" config user.name "Git Study"
git -C "$REPO" config user.email "git-study@example.invalid"

printf 'shared\n' > "$REPO/story.txt"
git -C "$REPO" add story.txt
git -C "$REPO" commit -q -m "base"
git -C "$REPO" branch feature

printf '%s\n' '--- two branch names, one commit ---'
git -C "$REPO" show-ref --heads

git -C "$REPO" switch -q feature
printf 'feature line\n' > "$REPO/story.txt"
git -C "$REPO" commit -qam "feature changes line"

git -C "$REPO" switch -q main
printf 'main line\n' > "$REPO/story.txt"
git -C "$REPO" commit -qam "main changes line"

printf '%s\n' '--- graph before merge ---'
git -C "$REPO" log --graph --decorate --oneline --all
if git -C "$REPO" merge feature; then
  printf '%s\n' 'unexpected: merge did not conflict'
  exit 1
fi
printf '%s\n' '--- conflict state ---'
git -C "$REPO" status --short
sed -n '1,10p' "$REPO/story.txt"

printf 'main line\nfeature line\n' > "$REPO/story.txt"
git -C "$REPO" add story.txt
git -C "$REPO" commit -q -m "merge feature"
printf '%s\n' '--- graph after resolution ---'
git -C "$REPO" log --graph --decorate --oneline --all
printf 'lab kept at: %s\n' "$LAB_DIR"
