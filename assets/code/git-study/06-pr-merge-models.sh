#!/usr/bin/env bash
set -eu

LAB_DIR="$(mktemp -d "${TMPDIR:-/tmp}/git-pr-models.XXXXXX")"
SOURCE="$LAB_DIR/source"
git init -q -b main "$SOURCE"
git -C "$SOURCE" config user.name "Git Study"
git -C "$SOURCE" config user.email "git-study@example.invalid"
printf 'base\n' > "$SOURCE/app.txt"
git -C "$SOURCE" add app.txt
git -C "$SOURCE" commit -q -m "base"
git -C "$SOURCE" switch -q -c feature
printf 'base\nfeature A\n' > "$SOURCE/app.txt"
git -C "$SOURCE" commit -qam "feature A"
printf 'base\nfeature A\nfeature B\n' > "$SOURCE/app.txt"
git -C "$SOURCE" commit -qam "feature B"
git -C "$SOURCE" switch -q main
printf 'main work\n' > "$SOURCE/main.txt"
git -C "$SOURCE" add main.txt
git -C "$SOURCE" commit -q -m "main work"

for MODE in merge squash rebase; do
  git clone -q "$SOURCE" "$LAB_DIR/$MODE"
  git -C "$LAB_DIR/$MODE" config user.name "Git Study"
  git -C "$LAB_DIR/$MODE" config user.email "git-study@example.invalid"
  git -C "$LAB_DIR/$MODE" switch -q -c feature origin/feature
done

git -C "$LAB_DIR/merge" switch -q main
git -C "$LAB_DIR/merge" merge -q --no-ff feature -m "Merge pull request model"

git -C "$LAB_DIR/squash" switch -q main
git -C "$LAB_DIR/squash" merge -q --squash feature
git -C "$LAB_DIR/squash" commit -q -m "feature A and B"

git -C "$LAB_DIR/rebase" rebase -q main
git -C "$LAB_DIR/rebase" switch -q main
git -C "$LAB_DIR/rebase" merge -q --ff-only feature

MERGE_TREE="$(git -C "$LAB_DIR/merge" rev-parse 'main^{tree}')"
SQUASH_TREE="$(git -C "$LAB_DIR/squash" rev-parse 'main^{tree}')"
REBASE_TREE="$(git -C "$LAB_DIR/rebase" rev-parse 'main^{tree}')"
test "$MERGE_TREE" = "$SQUASH_TREE"
test "$MERGE_TREE" = "$REBASE_TREE"

for MODE in merge squash rebase; do
  printf '%s\n' "--- local model of $MODE ---"
  git -C "$LAB_DIR/$MODE" log --graph --oneline main
  printf 'tree: '
  git -C "$LAB_DIR/$MODE" rev-parse 'main^{tree}'
done
printf 'verified identical main trees: %s\n' "$MERGE_TREE"
printf '%s\n' 'These are local history models, not GitHub PR executions.'
printf 'lab kept at: %s\n' "$LAB_DIR"
