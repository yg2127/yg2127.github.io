---
layout: post
title: "SQLite 2강 - Alias, VIEW, CASE, 윈도우 함수, JOIN"
description: "WHERE랑 HAVING 차이가 뭐야? 윈도우 함수는 왜 필요해? — SQL이 어려워지기 시작하는 지점"
date: 2026-02-21
category: "SQL"
tags: sql, sqlite, join, view, window-function
comments: true
---

## 들어가며

1강에서 CRUD 기초를 배우고 나니 "SQL 별거 아니네" 싶었는데, 2강부터 난이도가 확 올라갔다. VIEW, CASE, 윈도우 함수, JOIN까지 한 번에 나오니까 정신이 없었다. 특히 Python이나 C와는 패러다임이 달라서 "이걸 왜 이렇게 해야 하지?"라는 생각이 자주 들었다.

---

## 컬럼 별명 (Alias)

조회 결과에서 컬럼명을 다른 이름으로 표시할 수 있다. `AS` 키워드를 쓰거나 생략해도 된다.

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

계산식에도 별명을 붙일 수 있다:

```sql
SELECT
    Name,
    Height,
    Weight,
    round(weight / (height * height * 0.0001), 1) BMI
FROM Person;
```

`round(값, 자릿수)` — 소수점 이하 반올림:

```sql
SELECT round(123.4567, 2);  -- 123.46
```

---

## 뷰 (VIEW)

SELECT 문에 이름을 붙여서 저장해두는 것이다. 뷰를 일반 테이블처럼 조회할 수 있어서, 복잡한 쿼리를 재사용할 때 편하다.

`substr(문자열, 시작위치, 길이)` — 문자열 일부를 잘라내는 함수:

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

---

## 조건절 (CASE)

SQL의 if문이라고 생각하면 된다. 조건에 따라 다른 값을 반환한다:

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

> SQLite는 Oracle의 `DECODE()` 함수를 제공하지 않으므로 `CASE`를 사용한다.

---

## 날짜와 시간

SQLite에는 날짜/시간 자료형이 따로 없다. 대신 `strftime(포맷, 시간값, [수정자])` 함수를 사용한다:

```sql
-- 세계표준시(UTC)로 조회
SELECT strftime('%Y-%m-%d %H:%M:%S', 'now') 현재시간;

-- 현지시간으로 조회
SELECT strftime('%Y-%m-%d %H:%M:%S', 'now', 'localtime') 현지시간;
```

나이 구하기 — 비교 연산이 참이면 1, 거짓이면 0을 반환하는 것을 활용:

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

이 비교 연산으로 0/1을 빼는 트릭이 처음에 좀 낯설었는데, 생일이 아직 안 지났으면 1을 빼서 나이를 보정하는 방식이다.

```sql
SELECT CURRENT_DATE, CURRENT_TIME, CURRENT_TIMESTAMP;
```

---

## 집계 함수

- `count(*)` — NULL 포함 전체 행 수 / `count(컬럼)` — NULL 제외 행 수
- `max()` / `min()` — 최대/최소
- `sum()` — 합계
- `avg()` — 평균 (NULL 행 제외)

```sql
SELECT count(*) FROM Person;           -- 4
SELECT count(Height) FROM Person;      -- 3 (NULL 제외)
SELECT max(Height) FROM Person;        -- 170.3
SELECT min(Height) FROM Person;        -- 164
SELECT sum(Height) FROM Person;        -- 501.3
SELECT avg(Height) FROM Person;        -- 167.1
```

> `avg()`는 NULL이 있는 행을 **제외하고** 평균을 계산한다. 이걸 모르면 기대와 다른 결과가 나온다.

---

## 그룹화 (GROUP BY, HAVING)

`GROUP BY` — 같은 값끼리 묶어서 집계한다:

```sql
SELECT round(Height), count(*)
FROM   Person
GROUP  BY 1;
```

`HAVING` — 그룹핑한 **결과에** 조건을 거는 절:

```sql
SELECT round(Height), count(*)
FROM   Person
GROUP  BY round(Height)
HAVING count(*) > 1;
```

---

## OVER (윈도우 함수)

GROUP BY가 행을 합쳐서 줄이는 반면, `over()`는 **행 수를 유지하면서** 집계 결과를 각 행에 붙여준다.

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

> `over ()`는 SQLite 3.25 버전 이후에서 동작한다. `select sqlite_version();`으로 확인 가능.

---

## JOIN — 테이블 연결

`INNER JOIN` — 두 테이블에서 조건이 일치하는 행만 결합:

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

WHERE로도 같은 결과를 얻을 수 있다:

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

---

## UNION ALL과 UNION

```sql
SELECT 제목 FROM 음반
UNION ALL
SELECT 제목 FROM 노래
```

`UNION ALL`은 중복 포함, `UNION`은 중복 제거 + 정렬:

```sql
SELECT 제목 FROM 음반
UNION
SELECT 제목 FROM 노래
```

> 중복 제거가 필요 없으면 `UNION ALL`이 메모리와 속도에서 유리하다.

---

## 이미지 (BLOB)

이미지도 BLOB(Binary Large Object) 타입으로 데이터베이스에 저장할 수 있다:

```sql
CREATE TABLE `Images` (
    `name`  TEXT,
    `image` BLOB
);
```

DB Browser에서 드래그 앤 드롭으로 이미지를 넣고, Mode를 Image로 바꾸면 확인 가능하다.

---

## 헷갈리기 쉬운 포인트

### WHERE vs HAVING — 실행 시점이 다르다

- **WHERE**: 그룹핑 **전에** 행을 필터링
- **HAVING**: 그룹핑 **후에** 그룹을 필터링

```sql
-- WHERE: 키가 165 이상인 사람만 먼저 골라서 그룹핑
SELECT round(Height), count(*) FROM Person WHERE Height >= 165 GROUP BY 1;

-- HAVING: 전부 그룹핑한 후, 2명 이상인 그룹만 남기기
SELECT round(Height), count(*) FROM Person GROUP BY 1 HAVING count(*) > 1;
```

### JOIN의 ON과 WHERE 차이

INNER JOIN에서는 ON과 WHERE의 결과가 같지만, **LEFT JOIN에서는 달라진다.** ON 조건에 안 맞는 행도 NULL로 남기는 게 LEFT JOIN인데, WHERE로 필터링하면 NULL 행이 사라져버린다.

### 윈도우 함수는 언제 쓰나?

"각 행에 전체 평균을 같이 보여주고 싶다" → GROUP BY로는 행이 합쳐져서 불가능, 윈도우 함수로 해결. 편차, 순위, 누적합 같은 분석에 핵심이다.

---

## 정리하며

1. WHERE는 그룹핑 전, HAVING은 그룹핑 후 필터링
2. 윈도우 함수(`over()`)는 행을 유지하면서 집계 — GROUP BY와 근본적으로 다르다
3. JOIN은 테이블 간 관계를 연결하는 관계형 DB의 핵심이다
