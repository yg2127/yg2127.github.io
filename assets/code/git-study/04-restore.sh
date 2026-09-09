#!/usr/bin/env bash
set -eu

LAB_DIR="$(mktemp -d "${TMPDIR:-/tmp}/git-restore.XXXXXX")"
BASE="$LAB_DIR/base"
git init -q -b main "$BASE"
git -C "$BASE" config user.name "Git Study"
git -C "$BASE" config user.email "git-study@example.invalid"
for VERSION in 1 2 3; do
  printf 'version %s\n' "$VERSION" > "$BASE/note.txt"
  git -C "$BASE" add note.txt
  git -C "$BASE" commit -q -m "version $VERSION"
done

for NAME in restore staged soft mixed hard revert; do
  git clone -q "$BASE" "$LAB_DIR/$NAME"
  git -C "$LAB_DIR/$NAME" config user.name "Git Study"
  git -C "$LAB_DIR/$NAME" config user.email "git-study@example.invalid"
done

printf 'unfinished\n' > "$LAB_DIR/restore/note.txt"
git -C "$LAB_DIR/restore" restore note.txt
printf '%s\n' '--- restore: file content and status ---'
sed -n '1,5p' "$LAB_DIR/restore/note.txt"
git -C "$LAB_DIR/restore" status --short

printf 'staged edit\n' > "$LAB_DIR/staged/note.txt"
git -C "$LAB_DIR/staged" add note.txt
git -C "$LAB_DIR/staged" restore --staged note.txt
printf '%s\n' '--- restore --staged ---'
git -C "$LAB_DIR/staged" status --short

git -C "$LAB_DIR/soft" reset -q --soft HEAD~1
printf '%s\n' '--- reset --soft ---'
git -C "$LAB_DIR/soft" status --short

git -C "$LAB_DIR/mixed" reset -q HEAD~1
printf '%s\n' '--- reset --mixed ---'
git -C "$LAB_DIR/mixed" status --short

printf 'uncommitted and lost\n' > "$LAB_DIR/hard/note.txt"
git -C "$LAB_DIR/hard" reset -q --hard HEAD~1
printf '%s\n' '--- reset --hard ---'
sed -n '1,5p' "$LAB_DIR/hard/note.txt"
git -C "$LAB_DIR/hard" status --short

git -C "$LAB_DIR/revert" revert --no-edit HEAD >/dev/null
printf '%s\n' '--- revert log ---'
git -C "$LAB_DIR/revert" log --oneline -3
printf 'file after revert: '
sed -n '1p' "$LAB_DIR/revert/note.txt"
printf 'lab kept at: %s\n' "$LAB_DIR"
