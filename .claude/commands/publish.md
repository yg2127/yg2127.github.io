블로그 변경사항을 main 브랜치에 배포한다.

## 워크플로우

### 1단계: 상태 확인
- `git status`로 변경된 파일 목록 확인
- `git diff`로 변경 내용 확인
- 변경사항이 없으면 "배포할 변경사항이 없습니다" 출력 후 종료

### 2단계: 커밋
- 변경된 파일들을 stage (`.env`, credentials 등 민감 파일 제외)
- 변경 내용을 분석하여 적절한 커밋 메시지 자동 생성
- 커밋 메시지 컨벤션: `feat:`, `fix:`, `chore:` 등 사용
- Co-Authored-By 포함

### 3단계: Pull & Push
- `git pull --rebase origin main`으로 원격 변경사항 반영
- 충돌 발생 시 해결 시도, 실패하면 사용자에게 알림
- `git push origin main`으로 배포

### 4단계: 결과 보고
- push 성공/실패 여부
- 배포된 파일 목록
- GitHub Pages 반영 URL: https://yg2127.github.io
