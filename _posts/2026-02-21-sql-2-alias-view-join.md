---
layout: post
title: "SQLite 2강 - Alias, VIEW, CASE, 윈도우 함수, JOIN"
description: "컬럼 별명, 뷰, 조건절, 날짜 함수, 집계 함수, 그룹화, 윈도우 함수, JOIN, UNION"
date: 2026-02-21
category: "SQL"
tags: sql, sqlite, join, view, window-function
comments: true
---

## 잘한 점

### 상황 1.

- **상황**: SQL 실습 환경으로 DB Browser for SQLite를 사용하고 있었다
- **액션**: DB Browser 대신 VS Code에서 SQL을 실행하도록 환경을 변경했다
- **칭찬**: 가독성이 훨씬 좋아져서 코드 작성과 결과 확인이 편해졌다

---

## 개선점

### 상황 1.

- **문제**: 관계형 데이터베이스 개념이 어렵게 느껴진다
- **원인**: 기존에 다뤘던 Python이나 C언어와는 패러다임이 달라서 익숙하지 않다
- **액션플랜**: 각 개념(JOIN, 윈도우 함수 등)을 예제와 함께 반복 학습하기

---

## 배운 점

### 상황 1. 컬럼 별명 (Alias)

- **배움**: 조회 결과에서 컬럼명을 다른 이름으로 표시할 수 있다. `AS` 키워드를 사용하거나 생략하고 공백으로 구분해도 된다.

```sql
-- AS 키워드로 별명 지정
SELECT
    Name AS "이름",
    Birthday AS "생일"
FROM Person;

-- AS 생략 가능
SELECT
    Name "이름",
    Birthday "생일"
FROM Person;
```

계산식 결과에도 별명을 붙일 수 있다.

```sql
SELECT
    Name,
    Height,
    Weight,
    round(weight / (height * height * 0.0001), 1) BMI
FROM Person;
```

`round(값, 자릿수)` — 소수점 이하에 대해 반올림을 수행한다.

```sql
SELECT round(123.4567, 2);  -- 123.46
```

- **의미**: Alias는 SQL 결과의 가독성을 높이는 기본 도구이므로 확실히 익혀두고 싶다

### 상황 2. 뷰 (VIEW)

- **배움**: SELECT 문을 미리 만들어서 이름을 붙여둔 것이다. 뷰를 일반 테이블처럼 조회할 수 있다.

`substr(문자열, 시작위치, 길이)` — 문자열의 일부를 잘라서 반환한다.

```sql
SELECT
    substr('abcdefg', 3),     -- 셋째 자리부터 끝까지 → 'cdefg'
    substr('abcdefg', 3, 2);  -- 셋째 자리부터 두 글자 → 'cd'
```

```sql
CREATE VIEW BirthdayView
AS
SELECT
    Name,
    Birthday bdate,
    substr(Birthday, 1, 4) YYYY,
    substr(Birthday, 6, 2) MM,
    substr(Birthday, 9, 2) DD
FROM Person;

-- 뷰를 일반 테이블처럼 조회
SELECT * FROM BirthdayView;
```

- **의미**: 복잡한 SELECT 문을 뷰로 저장하면 재사용이 가능하므로, 반복 쿼리를 줄이는 핵심 개념이다

### 상황 3. 조건절 (CASE)

- **배움**: SQL의 CASE는 절차형 프로그래밍의 조건분기와 똑같지 않다. 오히려 함수형 언어의 분기문과 유사하다.

```sql
CASE
    WHEN 조건 THEN 값
    WHEN 조건 THEN 값
    ...
    [ELSE 값]
END
```

월 숫자를 영어 약어로 변환하는 예시:

```sql
SELECT
    Name,
    bdate,
    MM,
    CASE
        WHEN MM = '01' THEN 'Jan.'
        WHEN MM = '02' THEN 'Feb.'
        WHEN MM = '03' THEN 'Mar.'
        WHEN MM = '04' THEN 'Apr.'
        WHEN MM = '05' THEN 'May.'
        WHEN MM = '06' THEN 'Jun.'
        WHEN MM = '07' THEN 'Jul.'
        WHEN MM = '08' THEN 'Aug.'
        WHEN MM = '09' THEN 'Sep.'
        WHEN MM = '10' THEN 'Oct.'
        WHEN MM = '11' THEN 'Nov.'
        WHEN MM = '12' THEN 'Dec.'
    END Month
FROM BirthdayView;
```

> SQLite는 Oracle의 `DECODE()` 함수를 제공하지 않으므로 `CASE` 절을 사용하자.

- **의미**: CASE는 SQL에서 데이터 변환의 핵심이므로 패턴을 외워두면 좋겠다

### 상황 4. 날짜와 시간

- **배움**: SQLite에는 날짜/시간 자료형이 없지만, 관련 함수를 사용할 수 있다.

`strftime(포맷, 시간값, [수정자])` — 날짜/시간 값을 원하는 포맷 문자열로 변환한다.

```sql
-- 세계표준시(UTC)로 조회
SELECT strftime('%Y-%m-%d %H:%M:%S', 'now') 현재시간;

-- 현지시간으로 조회
SELECT strftime('%Y-%m-%d %H:%M:%S', 'now', 'localtime') 현지시간;
```

나이 구하기 — 비교 연산이 참이면 `1`, 거짓이면 `0`으로 반환되는 것을 활용한다.

```sql
SELECT
    Birthday "생일",
    strftime('%Y', 'now') - substr(Birthday, 1, 4) - (strftime('%m-%d', 'now') < substr(Birthday, 6)) "나이"
FROM Person
WHERE Name = '혜리';
```

```sql
-- 비교 연산 확인
SELECT
    '05-01' < '06-09' AS "5월 1일에 실행한 경우",  -- 1 (참)
    '07-01' < '06-09' AS "7월 1일에 실행한 경우"   -- 0 (거짓)
```

> 비교 연산(`<`)이 사칙연산보다 먼저 실행되었음에 유의하자.

SQLite에서는 날짜 관련 리터럴 값도 제공한다.

```sql
SELECT CURRENT_DATE, CURRENT_TIME, CURRENT_TIMESTAMP;
```

- **의미**: 날짜 계산에서 비교 연산의 0/1 반환을 활용하는 트릭이 인상적이었다

### 상황 5. 집계 함수

- **배움**: Person 테이블을 가지고 집계(aggregation) 함수를 사용해본다.

- `count(*)` — NULL 포함 전체 행 수 / `count(컬럼)` — NULL 제외 행 수
- `max()` — 최댓값 / `min()` — 최솟값
- `sum()` — 합계
- `avg()` — 평균값 (NULL 행 제외)

```sql
SELECT count(*) FROM Person;           -- 4
SELECT count(Height) FROM Person;      -- 3 (NULL 제외)
SELECT max(Height) FROM Person;        -- 170.3
SELECT min(Height) FROM Person;        -- 164
SELECT sum(Height) FROM Person;        -- 501.3
SELECT avg(Height) FROM Person;        -- 167.1
```

> `avg()`는 NULL 값이 빠진 행을 제외하고 평균을 계산한다.

- **의미**: 집계 함수에서 NULL 처리 방식을 이해해야 정확한 결과를 얻을 수 있다

### 상황 6. 그룹화

- **배움**: `GROUP BY` — 같은 값을 가진 행들을 하나의 그룹으로 묶어 집계 함수를 적용한다.

```sql
SELECT round(Height), count(*)
FROM   Person
GROUP  BY 1;
```

- `GROUP BY 1`은 첫 번째 컬럼을 가리킨다

`HAVING` — 그룹핑한 결과에 조건을 거는 절이다 (WHERE는 그룹핑 전, HAVING은 그룹핑 후).

```sql
SELECT round(Height), count(*)
FROM   Person
GROUP  BY round(Height)
HAVING count(*) > 1;
```

> `HAVING` 절에 사용한 숫자 `1`은 리터럴 값임에 유의하라.

- **의미**: WHERE vs HAVING의 실행 시점 차이를 기억해두면 쿼리 작성 시 혼동을 줄일 수 있다

### 상황 7. OVER (윈도우 함수)

- **배움**: `over ()` — 원래 행을 유지하면서 집계 함수의 결과를 각 행에 함께 나타내는 윈도우 함수이다. GROUP BY는 행을 합쳐서 줄이지만, over ()는 행 수를 그대로 유지한다.

통계량 정의:
- **편차**(deviation): 관측값에서 평균을 뺀 것
- **분산**(variance): 편차를 제곱하고 모두 더한 후 전체 개수로 나눈 것
- **표준편차**(standard deviation): 분산을 제곱근한 것

```sql
-- 평균을 각 행에 함께 표시
SELECT
    Name AS 이름,
    Height AS 키,
    avg(Height) over () AS 평균
FROM Person
```

```sql
-- 편차 구하기
SELECT
    Name AS 이름,
    Height AS 키,
    avg(Height) over () AS 평균,
    round(Height - avg(Height) over (), 3) AS 편차
FROM Person
```

```sql
-- 편차의 합 확인 (0이 되는지)
SELECT sum(편차)
FROM (
    SELECT
        round(avg(Height) over () - height, 3) AS 편차
    FROM Person
)
-- 결과: 0.0
```

```sql
-- 분산 구하기
SELECT avg(편차*편차) AS 분산
FROM (
    SELECT
        round(avg(Height) over () - height, 3) AS 편차
    FROM Person
)
-- 결과: 4.981875
```

> `over ()`는 SQLite 3.25 버전 이후에서 작동한다. `select sqlite_version();` 으로 확인 가능.

- **의미**: 윈도우 함수는 GROUP BY와 달리 행을 유지하면서 집계할 수 있어 통계 분석에 필수적이다

### 상황 8. 세 테이블의 조인

- **배움**: `INNER JOIN` — 두 테이블에서 조건이 일치하는 행만 결합하여 조회한다.

```sql
CREATE TABLE 노래 (
  ID INTEGER NOT NULL PRIMARY KEY,
  제목 TEXT NOT NULL
);

CREATE TABLE 음반 (
  ID INTEGER NOT NULL PRIMARY KEY,
  제목 TEXT NOT NULL,
  연도 INTEGER
);

CREATE TABLE 수록곡 (
  음반ID INTEGER NOT NULL,
  노래ID INTEGER NOT NULL
);
```

```sql
INSERT INTO 노래 VALUES
(1, '가우뚱'), (2, 'Shuppy Shuppy'), (3, 'Control'),
(4, '영러브'), (5, '한번만 안아줘'), (6, '반짝반짝'),
(7, '기대해'), (8, 'I Don''t Mind'), (9, 'Easy go'),
(10, '여자대통령');

INSERT INTO 음반 VALUES
(1, 'Girl''s Day Party #1', 2010),
(2, 'Everyday', 2011),
(3, 'Expectation', 2013),
(4, '여자대통령', 2013);

INSERT INTO 수록곡 VALUES
(1, 1), (1, 2), (1, 3),
(2, 4), (2, 5), (2, 6),
(3, 7), (3, 8), (3, 9), (3, 6), (3, 5),
(4, 10);
```

```sql
-- INNER JOIN으로 세 테이블 조회
SELECT 음반.제목 앨범명, 음반.연도 발매년도, 노래.제목 곡명
FROM 수록곡
INNER JOIN 음반 ON 수록곡.음반ID = 음반.ID
INNER JOIN 노래 ON 수록곡.노래ID = 노래.ID
```

WHERE로도 같은 결과를 얻을 수 있다.

```sql
SELECT 음반.제목 앨범명, 음반.연도 발매년도, 노래.제목 곡명
FROM 노래, 음반, 수록곡
WHERE 음반.ID = 수록곡.음반ID AND 노래.ID = 수록곡.노래ID;
```

```sql
-- WHERE 절로 필터링 추가
SELECT 음반.제목 앨범명, 음반.연도 발매년도, 노래.제목 곡명
FROM 수록곡
INNER JOIN 음반 ON 수록곡.음반ID = 음반.ID
INNER JOIN 노래 ON 수록곡.노래ID = 노래.ID
WHERE 음반.연도 = 2011;
```

- **의미**: 관계형 데이터베이스의 핵심인 JOIN을 처음 다뤄봤는데, 테이블 간 관계를 연결하는 개념이 중요하다

### 상황 9. UNION ALL과 UNION

- **배움**: `UNION ALL` — 복수의 SELECT 결과를 하나로 합치는 집합 연산이다. 중복 레코드를 제거하지 않는다.

```sql
SELECT 제목 FROM 음반
UNION ALL
SELECT 제목 FROM 노래
```

`UNION` — UNION ALL과 비슷하지만, 중복 레코드를 제거한 결과를 돌려준다. 내부적으로 정렬을 수행한다.

```sql
SELECT 제목 FROM 음반
UNION
SELECT 제목 FROM 노래
```

> 정렬과 중복 제거가 굳이 필요하지 않을 때는 `UNION ALL`을 사용하는 것이 메모리와 속도 측면에서 유리하다.

CASE 절 대신 UNION ALL로 매핑 테이블을 만들어 조인할 수도 있다. RDBMS는 테이블 연산에 최적화되어 있어, 조건절보다 조인으로 처리하는 것이 낫다.

- **의미**: UNION과 UNION ALL의 성능 차이를 알아두면 실무에서 쿼리 최적화에 도움이 될 것이다

### 상황 10. 이미지 (BLOB)

- **배움**: 이미지를 데이터베이스에 저장할 수 있다. BLOB(Binary Large Object) 타입을 사용한다.

```sql
CREATE TABLE `Images` (
    `name`  TEXT,
    `image` BLOB
);
```

- DB Browser for SQLite에서 Browse Data 탭 → 드래그 앤 드롭으로 이미지 입력 가능
- Mode를 Image로 바꾸면 이미지 확인, Binary로 바꾸면 바이너리 데이터 확인 가능

- **의미**: 데이터베이스에 바이너리 데이터도 저장할 수 있다는 점을 알게 되었다
