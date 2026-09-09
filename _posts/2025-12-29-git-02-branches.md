---
layout: post
title: "브랜치는 프로젝트 복사본이 아니다"
description: "브랜치를 프로젝트 복사본이라고 적었던 문장에서 출발해, 두 이름이 같은 커밋을 가리키고 갈라지는 과정을 실제 그래프로 확인한다"
date: 2025-12-29
category: Git
tags: [git, branch, switch, merge, conflict]
series: git-basics
series_title: Git 학습 기록
series_order: 2
permalink: /2026/09/git-02-branches.html
comments: false
---

## 복사본이라는 한 줄

`Branch` 노트의 첫 문장은 아주 짧다.

> Branch : 프로젝트 복사본 생성

main을 건드리지 않고 별도로 작업할 수 있다는 사용감은 잘 잡은 표현이다. 하지만 이 문장을 그대로 두면 branch를 만들 때 프로젝트 디렉터리 전체가 하나 더 복사된다고 이해하기 쉽다. 노트에 이어진 두 명령을 따라가면 실제 경계가 보인다.

```bash
git branch 브랜치1
git switch 브랜치1
```

노트에도 “branch로 브랜치 생성, switch로 브랜치 변경”이라고 구분돼 있다. 생성과 전환이 별도 명령이라는 점이 중요하다. `git branch feature`는 새 이름을 만들 뿐 working tree를 바꾸지 않는다. `git switch feature`를 실행하면 HEAD가 feature를 가리키고, 로컬 변경과 충돌하지 않는 범위에서 index와 working tree도 해당 commit에 맞춰진다. 전환으로 덮어쓰게 될 변경이 있으면 Git이 멈추므로, switch가 언제나 작업 중인 내용을 지우고 깨끗한 snapshot으로 만든다고 보면 안 된다. ([git-branch](https://git-scm.com/docs/git-branch), [git-switch](https://git-scm.com/docs/git-switch))

## 두 이름이 같은 곳을 가리키는 상태

이번에는 빈 저장소에 commit 하나를 만들고 `git branch feature`를 실행했다. 바로 `git show-ref --heads`로 두 이름이 가리키는 값을 확인했다.

```text
52323107883badccfbeeb0d268fd9c5a83f53d2a refs/heads/feature
52323107883badccfbeeb0d268fd9c5a83f53d2a refs/heads/main
```

두 branch의 hash가 완전히 같다. 파일 snapshot을 새로 복사한 것이 아니라 기존 commit에 `feature`라는 ref 하나를 더 만든 상태다. Pro Git이 branch를 “commit을 가리키는 가벼운 이동 포인터”로 설명하는 이유다. ([Branches in a Nutshell](https://git-scm.com/book/en/v2/Git-Branching-Branches-in-a-Nutshell))

이후 feature에서 commit하면 feature 포인터만 새 commit으로 이동한다. main으로 돌아와 다른 commit을 만들면 두 포인터가 각자의 끝을 가리킨다.

```text
* 5099066 (feature) feature changes line
| * b49bad4 (HEAD -> main) main changes line
|/
* 5232310 base
```

노트에 적힌 `git log --graph --oneline --all`이 이때 힘을 발휘한다. 여기에 `--decorate`를 더하면 commit뿐 아니라 main, feature, HEAD가 어디를 가리키는지도 함께 보인다. branch를 디렉터리 복사본보다 commit 그래프 위의 이름으로 생각하면, switch가 무엇을 옮기고 commit할 때 무엇이 전진하는지 한 그림에서 읽힌다.

## merge는 현재 브랜치에 들어온다

노트에는 merge의 방향도 정확하게 남아 있다.

> merge 사용시 현재 브랜치에 합치고싶은브랜치 를 반영한다.

main에서 `git merge feature`를 실행하면 움직이는 쪽은 현재 branch인 main이다. feature의 변경을 main history에 통합하고, feature라는 이름 자체를 main으로 바꾸는 명령은 아니다.

이번 실험에서는 두 branch가 `story.txt`의 같은 줄을 각각 `main line`, `feature line`으로 바꿨다. Git은 어느 값을 최종 결과로 고를지 알 수 없어서 다음 상태를 만들었다.

```text
Auto-merging story.txt
CONFLICT (content): Merge conflict in story.txt
Automatic merge failed; fix conflicts and then commit the result.

UU story.txt
```

파일에는 두 변경이 표시됐다.

```text
<<<<<<< HEAD
main line
=======
feature line
>>>>>>> feature
```

원문은 “`<<<<< ======= >>>>>>>` 이런 거 다 지우고 원하는 코드만 남기면 된다”고 정리한다. 표시를 제거하는 것만으로는 Git에 해결 완료가 전달되지 않는다. 먼저 `HEAD` 쪽과 feature 쪽 변경을 읽고 최종 파일을 만든 다음, `git add story.txt`로 해결된 내용을 index에 올리고 commit해야 한다. 합칠 방향을 다시 생각해야 한다면 `git merge --abort`로 merge 시작 전 상태로 돌아갈 수도 있다. ([git-merge](https://git-scm.com/docs/git-merge))

두 줄을 모두 남기고 commit한 뒤 그래프는 이렇게 바뀌었다.

```text
*   804a5b7 (HEAD -> main) merge feature
|\
| * 5099066 (feature) feature changes line
* | b49bad4 main changes line
|/
* 5232310 base
```

main은 두 갈래를 부모로 갖는 merge commit까지 전진했고 feature는 원래 끝에 그대로 있다. 여기서 branch의 성격이 다시 드러난다. branch는 별도 프로젝트 덩어리가 아니라 commit 하나를 가리키는 이름이므로 merge 뒤에도 자동으로 사라지지 않는다.

“프로젝트 복사본”이라는 첫 문장은 독립적으로 작업하는 느낌을 설명했지만 내부 구조까지 설명하지는 못했다. 현재의 정리는 이렇다. branch 생성은 commit에 새 이름을 붙이는 일이고, switch는 HEAD를 그 이름으로 옮기면서 가능한 범위에서 index와 working tree를 맞추는 일이며, merge는 현재 branch의 history에 다른 history를 통합하는 일이다.

같은 충돌과 그래프는 [02-branches.sh](/assets/code/git-study/02-branches.sh)에서 재현할 수 있다. 모든 변경은 `mktemp`가 만든 임시 저장소에서만 일어난다.
