#!/usr/bin/env bash
set -eu

LAB_DIR="$(mktemp -d "${TMPDIR:-/tmp}/git-history.XXXXXX")"
BASE="$LAB_DIR/base"
git init -q -b main "$BASE"
git -C "$BASE" config user.name "Git Study"
git -C "$BASE" config user.email "git-study@example.invalid"
printf 'base\n' > "$BASE/base.txt"
git -C "$BASE" add base.txt
git -C "$BASE" commit -q -m "base"
git -C "$BASE" branch feature
printf 'main work\n' > "$BASE/main.txt"
git -C "$BASE" add main.txt
git -C "$BASE" commit -q -m "main work"
git -C "$BASE" switch -q feature
printf 'feature one\n' > "$BASE/feature.txt"
git -C "$BASE" add feature.txt
git -C "$BASE" commit -q -m "feature one"
printf 'feature one\nfeature two\n' > "$BASE/feature.txt"
git -C "$BASE" commit -qam "feature two"
git -C "$BASE" switch -q main

for NAME in merge rebase squash; do
  git clone -q "$BASE" "$LAB_DIR/$NAME"
  git -C "$LAB_DIR/$NAME" config user.name "Git Study"
  git -C "$LAB_DIR/$NAME" config user.email "git-study@example.invalid"
  git -C "$LAB_DIR/$NAME" switch -q -c feature origin/feature
done

git -C "$LAB_DIR/merge" switch -q main
git -C "$LAB_DIR/merge" merge -q --no-ff feature -m "merge feature"

git -C "$LAB_DIR/rebase" rebase -q main
git -C "$LAB_DIR/rebase" switch -q main
git -C "$LAB_DIR/rebase" merge -q --ff-only feature

git -C "$LAB_DIR/squash" switch -q main
git -C "$LAB_DIR/squash" merge -q --squash feature
printf '%s\n' '--- squash before commit ---'
git -C "$LAB_DIR/squash" status --short
git -C "$LAB_DIR/squash" commit -q -m "squash feature"

for NAME in merge rebase squash; do
  printf '%s\n' "--- $NAME graph ---"
  git -C "$LAB_DIR/$NAME" log --graph --decorate --oneline --all
  printf 'tree: '
  git -C "$LAB_DIR/$NAME" rev-parse 'main^{tree}'
done
printf 'lab kept at: %s\n' "$LAB_DIR"
