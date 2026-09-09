---
layout: post
title: "병합 버튼은 기록의 모양을 정한다"
description: "PR의 merge, squash, rebase 선택을 같은 최종 파일과 다른 커밋 그래프로 비교하고, 브랜치 전략을 팀 규모가 아니라 변경 흐름으로 판단한다"
date: 2026-01-03
category: Git
tags: [git, github, pull-request, merge, workflow]
series: git-basics
series_title: Git 학습 기록
series_order: 6
permalink: /2026/09/git-06-workflow.html
comments: false
---

## merge하고 싶다고 신청하는 것

`GITHUB3` 노트에는 pull request가 아주 직접적인 말로 적혀 있다.

> 깃헙 웹사이트에선 pr (pull request)이 merge 하고싶다고 윗사람한테 신청하는게 있음. 그거 하면 된다.

PR을 처음 구분할 때 핵심은 들어 있다. feature branch의 변경을 base branch에 바로 밀어 넣지 않고, 변경 내용을 보여주고 검토한 뒤 통합한다. 다만 PR은 특정 직급의 사람에게만 보내는 기능은 아니다. 저장소의 권한과 review 규칙에 따라 동료나 maintainer가 확인하고, 조건을 만족하면 허용된 방식으로 merge한다.

노트는 이어서 merge 방식도 “3-way, squash, rebase” 중 선택할 수 있다고 적었다. 버튼을 누르면 최종 파일만 같아지는 것처럼 보이지만, 선택은 commit history의 모양을 정한다. GitHub 공식 문서는 현재 PR merge 방식을 merge commit, squash and merge, rebase and merge로 구분한다. ([GitHub pull request merges](https://docs.github.com/en/pull-requests/reference/pull-request-merges))

## 같은 변경을 세 가지 기록으로 남긴 모형

실제 GitHub PR을 만들거나 원격 서비스에 push하지 않고, 이번에는 로컬 저장소 세 벌로 각 버튼의 핵심 history 모양을 재현했다. base에서 feature branch를 나눠 `feature A`, `feature B` 두 commit을 만들고, main에도 겹치지 않는 `main work` commit을 하나 추가했다. 세 모형 모두 이 갈라진 상태에서 시작했다.

merge commit 방식의 그래프는 이랬다.

```text
*   0e2d7be Merge pull request model
|\
| * a0b798e feature B
| * 3575e8b feature A
* | 2a30ed9 main work
|/
* aab5045 base
```

feature의 개별 commit을 보존하면서 부모가 둘인 merge commit을 하나 더 만든다. GitHub의 기본 Merge pull request가 `--no-ff` 형태로 명시적인 merge commit을 만든다는 설명과 대응한다. 작업 branch가 언제 main에 들어왔는지 경계가 보이는 대신 그래프에는 가지가 남는다.

squash 모형은 두 commit의 변경을 하나로 준비한 뒤 새 commit을 만들었다.

```text
* 489d4c2 feature A and B
* 2a30ed9 main work
* aab5045 base
```

base branch에는 중간 commit 두 개 대신 논리적 변경 하나만 남는다. 작은 수정 commit이 많은 PR을 한 단위로 읽기 쉽지만, 개별 commit의 경계는 base history에서 사라진다.

rebase 모형은 각 feature commit을 base 위에 차례로 놓아 merge commit 없는 직선을 만들었다.

```text
* 4f428d1 feature B
* 572378f feature A
* 2a30ed9 main work
* aab5045 base
```

원래 feature commit은 `3575e8b`, `a0b798e`였지만 main의 새 base 위에 재적용된 commit은 `572378f`, `4f428d1`이 됐다. 부모가 달라져 새 commit SHA가 생기고, merge commit 없는 직선 history가 남았다. GitHub의 Rebase and merge는 committer 정보까지 갱신하는 등 로컬 `git rebase`와 세부 동작이 다르므로, 이 그래프를 GitHub 서버 동작의 완전한 복제라고 보지는 않는다. 핵심은 개별 commit을 새 base 위에 다시 놓아 선형 history를 만든다는 점이다.

세 `main^{tree}` 값은 모두 같았다.

```text
8d78c1086bb2464d33afe9d468afe7682c9f1d0f
```

최종 파일만으로는 버튼 선택의 차이를 알 수 없다. 작업의 중간 commit까지 의미가 있으면 merge나 rebase가 정보를 남기고, PR 전체가 하나의 변경 단위라면 squash가 history를 짧게 만든다.

## 모든 브랜치를 push한다는 메모의 수정

같은 노트에는 다음 두 줄도 있다.

> git push origin
> 위는 모든 브랜치에 대해 푸쉬하게 된다고

직접 인용은 당시 메모 그대로 두되 현재 명령의 뜻은 다르다. refspec을 생략한 `git push origin`의 동작은 remote에 설정한 push refspec과 `push.default`에 따라 정해진다. 별도 설정이 없는 기본 `simple` 모드에서는 보통 현재 branch를 같은 이름의 upstream branch로 push하며, 조건이 맞지 않으면 실패한다. 모든 local branch를 명시적으로 보내는 명령은 `git push --all origin`이다. ([git-push](https://git-scm.com/docs/git-push))

PR branch 하나를 올릴 때는 대상도 명시하는 편이 생각과 명령을 맞추기 쉽다.

```bash
git push -u origin feature/login
```

이 명령은 모든 branch가 아니라 로컬 `feature/login`을 push하고 그 branch의 upstream 관계를 설정한다.

## 브랜치 수보다 변경의 흐름을 보기

`GIT FLOW` 노트는 더 큰 질문으로 넘어간다.

> 개발자 100명이 되어도 누가 뭘 했는지 추적할 수 있고 branch, merge를 깔끔하게 하고 싶으면 협업 방법론을 배우면 된다 그래서 내가 팀을 이끌고 싶을 때 활용해야한다

이어 main, develop, feature, release, hotfix 역할을 나누고, trunk-based는 main과 작은 feature 단위로 설명한다. 여기서 살릴 생각은 branch 이름을 외우는 것보다 사람이 많아져도 변경의 목적과 이동 경로를 추적하고 싶다는 출발점이다.

다만 “개발자 100명”만으로 전략이 자동 결정되지는 않는다. 동시에 여러 release를 유지하는지, 배포 전 안정화 기간이 따로 필요한지, 변경을 얼마나 작게 나눌 수 있는지, 자동 테스트와 review가 얼마나 빨리 끝나는지를 함께 봐야 한다. Git Flow의 develop·release·hotfix는 단계와 장기 branch를 명시해 release 흐름을 분리한다. 반대로 trunk-based 흐름은 짧게 사는 branch와 잦은 통합을 전제로 하므로 main을 하나만 둔다는 사실보다 작은 변경, 빠른 검증, 미완성 기능을 숨길 장치가 중요하다.

노트의 “CI/CD 기반 개발팀에선 안좋아함”, “한 브랜치에서만 관리하기 때문에 편리함”도 절대적인 장단점으로 남기기에는 넓다. 오래 사는 branch가 많으면 자동 배포까지 가는 통합 간격이 길어질 수 있고, 짧은 branch는 통합 부담을 줄이는 대신 테스트와 review 실패를 빨리 잡는 체계가 필요하다. 팀의 이름보다 변경이 머무는 시간을 기준으로 비교해야 한다.

결국 PR의 merge 버튼과 branch 전략은 따로 떨어진 규칙이 아니다. 어떤 변경 단위를 검토하고, 어느 history를 보존하며, 언제 main에 통합할지를 정하는 한 흐름이다. 버튼 이름보다 남겨야 할 기록을 먼저 정하면 merge, squash, rebase의 선택도 설명할 수 있다.

세 PR 방식의 로컬 history 모형은 [06-pr-merge-models.sh](/assets/code/git-study/06-pr-merge-models.sh)에서 확인할 수 있다. 이 스크립트는 GitHub PR을 실행하지 않으며 `mktemp` 아래의 로컬 저장소만 사용한다. commit SHA는 실행 시각에 따라 달라질 수 있으므로 숫자 자체보다 부모 관계, rebase 전후 SHA의 변화, 세 `main` tree의 일치를 확인하면 된다.
