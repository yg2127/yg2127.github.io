Obsidian 노트를 블로그 포스트로 변환하는 워크플로우를 실행해줘.

## 입력 파일
$ARGUMENTS

인자가 비어있으면 사용자에게 Obsidian 파일 경로를 물어봐.
Vault 기본 경로: `/Users/yugeon/Library/Mobile Documents/iCloud~md~obsidian/Documents/Vault1`

## 워크플로우

### 1단계: 파일 읽기
- 입력된 Obsidian 파일을 읽는다
- 기존 블로그 포스트 하나를 읽어서 형식 참고

### 2단계: TIL 내용 수집
사용자에게 아래 내용을 물어본다:
- **잘한 점** (상황/액션/칭찬): 오늘 학습에서 잘한 것
- **개선점** (문제/원인/액션플랜): 어려웠던 점, 개선할 점
- 없으면 없다고 해도 OK

### 3단계: 포스트 작성
CLAUDE.md의 Blog Post Template (TIL 형식)을 따라 작성:
- Obsidian front matter 제거 (작성일자, tags, parent, type, status)
- Obsidian Tasks 섹션 제거
- Jekyll front matter 생성 (layout, title, description, date, category, tags, comments)
- TIL 3섹션 구조: 잘한 점 / 개선점 / 배운 점
- 배운 점에 강의 내용 배치 (각 항목: 배움/의미)
- 테이블 사용하지 않고 리스트로 대체
- 원본 텍스트 최대한 유지, 어미만 ~다 체로 통일

### 4단계: 미리보기
- 작성된 포스트 전체 내용을 사용자에게 보여준다
- 수정 요청이 있으면 반영

### 5단계: 포스팅
- 사용자 확인 후 `_posts/` 디렉토리에 파일 생성
- 파일명: `YYYY-MM-DD-slug.md`

### 6단계: 배포
- git add, commit, push origin main 실행
- 원격에 새 커밋이 있으면 pull --rebase 후 push
