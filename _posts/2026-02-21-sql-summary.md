---
layout: post
title: "SQLite 총정리 - 코드와 정의 치트시트"
description: "CRUD부터 JOIN·윈도우 함수까지, SQLite 입문 핵심을 코드와 정의로 한 번에 복습"
date: 2026-02-21
category: "SQL"
tags: sql, sqlite, cheatsheet, summary
comments: true
---

SQLite 입문 시리즈(기초·CRUD / VIEW·JOIN·윈도우 함수 / 실데이터 분석)를 코드와 정의 위주로 압축한 정리.

---

## 0. 개요 / 환경

- SQL = 관계형 DB 조작 표준 언어. **선언형**("뭘 원하는지"를 기술)
- SQLite = 서버 없이 파일 하나(`.db`)로 동작하는 경량 DB
- Mac 내장 `sqlite3`, GUI는 **DB Browser for SQLite** (`brew install --cask db-browser-for-sqlite`)
- Python: `import sqlite3` (별도 설치 불필요)
- 버전 확인: `SELECT sqlite_version();`

---

## 1. 테이블 (CREATE / DROP / ALTER)

```sql
CREATE TABLE IF NOT EXISTS Person (
    ID       INTEGER PRIMARY KEY AUTOINCREMENT,
    Name     TEXT NOT NULL,
    Birthday TEXT
);

DROP TABLE IF EXISTS Person;

ALTER TABLE Person ADD COLUMN Email TEXT;            -- 컬럼 추가
ALTER TABLE Person ADD COLUMN Age INTEGER DEFAULT 0; -- NOT NULL 추가 시 DEFAULT 필수
ALTER TABLE Person DROP COLUMN Email;                -- 3.35.0+
ALTER TABLE Person RENAME COLUMN Birthday TO BirthDate; -- 3.25.0+
```

### 데이터 타입

| 타입 | 설명 |
|---|---|
| `INTEGER` | 정수 |
| `TEXT` | 문자열 |
| `REAL` | 실수(소수점) |
| `BLOB` | 바이너리(이미지·파일) |
| `NULL` | 값 없음 |

### 제약조건

| 제약조건 | 설명 |
|---|---|
| `PRIMARY KEY` | 행 고유 식별 대표 키 |
| `AUTOINCREMENT` | 자동 번호 매기기 |
| `NOT NULL` | NULL 불허 |
| `DEFAULT` | 기본값 |
| `UNIQUE` | 중복 불허 |
| `CHECK(조건)` | 값 범위 제한 |
| `FOREIGN KEY ... REFERENCES` | 다른 테이블 참조 |

---

## 2. CRUD

```sql
-- Create
INSERT INTO Person (Name, Birthday) VALUES ('안태희', '2001-02-10');
INSERT INTO Person (Name, Birthday) VALUES ('유진','2001-01-01'), ('최시원','2000-01-01');

-- Read
SELECT * FROM Person;
SELECT Name, Age FROM Person WHERE Age > 20 ORDER BY Age DESC LIMIT 10;

-- Update
UPDATE Person SET Name = '이영희' WHERE ID = 2;

-- Delete
DELETE FROM Person WHERE ID = 3;
```

| 작업 | 기본 구조 |
|---|---|
| 추가 | `INSERT INTO 테이블 (컬럼) VALUES (값)` |
| 조회 | `SELECT 컬럼 FROM 테이블 WHERE 조건` |
| 갱신 | `UPDATE 테이블 SET 컬럼=값 WHERE 조건` |
| 삭제 | `DELETE FROM 테이블 WHERE 조건` |

- **UPDATE/DELETE에 `WHERE` 빼먹으면 전체 행이 수정·삭제됨** → 실행 전 같은 조건으로 SELECT 먼저
- `DELETE` = 데이터만 삭제(구조 유지) / `DROP TABLE` = 테이블 자체 삭제

---

## 3. WHERE 연산자

```sql
WHERE Age = 20 / != / > / >= / < / <=          -- 비교
WHERE Age >= 20 AND Name = '홍길동'             -- 논리 AND / OR / NOT
WHERE Age BETWEEN 20 AND 30                      -- 범위 (이상~이하)
WHERE Name IN ('홍길동', '김철수')                -- 목록
WHERE Name LIKE '김%'   -- 시작 / '%수' 끝 / '%길%' 포함  (% 패턴)
WHERE Birthday IS NULL / IS NOT NULL             -- NULL 판별
```

---

## 4. 정렬·그룹·집계

```sql
SELECT * FROM Person ORDER BY Age ASC;   -- 오름차순(ASC) / 내림차순(DESC)

SELECT Age, COUNT(*) FROM Person GROUP BY Age;                  -- 그룹별 집계
SELECT Age, COUNT(*) FROM Person GROUP BY Age HAVING COUNT(*) >= 2;  -- 그룹 필터
```

| 함수 | 뜻 | 비고 |
|---|---|---|
| `COUNT(*)` | 전체 행 수 | NULL 포함 |
| `COUNT(컬럼)` | 행 수 | NULL 제외 |
| `COUNT(DISTINCT 컬럼)` | 고유 값 종류 수 | |
| `SUM` / `AVG` | 합계 / 평균 | AVG는 NULL 행 제외 |
| `MAX` / `MIN` | 최대 / 최소 | |

- **WHERE = 그룹핑 전 행 필터 / HAVING = 그룹핑 후 그룹 필터**

---

## 5. Alias (별명)

```sql
SELECT Name AS "이름", Birthday "생일" FROM Person;   -- AS 생략 가능
SELECT Name, round(Weight / (Height*Height*0.0001), 1) BMI FROM Person;
SELECT round(123.4567, 2);   -- 123.46  (round(값, 자릿수))
```

---

## 6. VIEW (저장된 SELECT)

```sql
CREATE VIEW BirthdayView AS
SELECT Name,
       substr(Birthday, 1, 4) YYYY,   -- substr(문자열, 시작, 길이)
       substr(Birthday, 6, 2) MM,
       substr(Birthday, 9, 2) DD
FROM Person;

SELECT * FROM BirthdayView;   -- 일반 테이블처럼 조회
```

- `substr('abcdefg', 3)` → `'cdefg'` / `substr('abcdefg', 3, 2)` → `'cd'`

---

## 7. CASE (SQL의 if문)

```sql
SELECT Name,
       CASE
           WHEN MM = '01' THEN 'Jan.'
           WHEN MM = '02' THEN 'Feb.'
           ELSE '기타'
       END Month
FROM BirthdayView;
```

- SQLite는 Oracle의 `DECODE()`가 없어 `CASE` 사용

---

## 8. 날짜·시간

```sql
SELECT strftime('%Y-%m-%d %H:%M:%S', 'now');               -- UTC
SELECT strftime('%Y-%m-%d %H:%M:%S', 'now', 'localtime');  -- 현지시간
SELECT CURRENT_DATE, CURRENT_TIME, CURRENT_TIMESTAMP;
```

- SQLite는 날짜 자료형이 없음 → `strftime()` / `substr()`로 텍스트 처리
- 비교 연산은 참=1, 거짓=0 반환 → 나이 보정 등에 활용
  (`'05-01' < '06-09'` → `1`)

---

## 9. 윈도우 함수 OVER()

```sql
SELECT Name 이름, Height 키,
       avg(Height) over () AS 평균,                  -- 행 수 유지하며 집계 부착
       round(Height - avg(Height) over (), 3) AS 편차
FROM Person;
```

- `GROUP BY`는 행을 합쳐 줄이지만, `over()`는 **행 수를 유지**하며 집계값을 각 행에 부착
- 편차·순위·누적합 분석의 핵심 (SQLite 3.25+)

---

## 10. JOIN

```sql
SELECT 음반.제목 앨범명, 노래.제목 곡명
FROM 수록곡
INNER JOIN 음반 ON 수록곡.음반ID = 음반.ID
INNER JOIN 노래 ON 수록곡.노래ID = 노래.ID
WHERE 음반.연도 = 2011;

-- WHERE 절로 같은 결과 (콤마 조인)
SELECT * FROM 노래, 음반, 수록곡
WHERE 음반.ID = 수록곡.음반ID AND 노래.ID = 수록곡.노래ID;
```

- `INNER JOIN` = 양쪽 조건 일치 행만 결합
- INNER JOIN은 `ON`과 `WHERE` 결과가 같지만, **LEFT JOIN에서는 다름**
  (ON은 NULL 행 유지, WHERE는 NULL 행 제거)

---

## 11. UNION / UNION ALL

```sql
SELECT 제목 FROM 음반 UNION ALL SELECT 제목 FROM 노래;  -- 중복 포함
SELECT 제목 FROM 음반 UNION     SELECT 제목 FROM 노래;  -- 중복 제거 + 정렬
```

- 중복 제거 불필요하면 `UNION ALL`이 속도·메모리 유리

---

## 12. BLOB (이미지 저장)

```sql
CREATE TABLE Images ( name TEXT, image BLOB );
```

- DB Browser에서 드래그 앤 드롭 후 Mode를 Image로 변경하면 확인 가능

---

## 13. 실전 패턴 (CSV 분석)

```python
# CSV → DB 마이그레이션 (pandas + sqlite3)
import pandas as pd, sqlite3
conn = sqlite3.connect('./bird.db')          # 없으면 자동 생성
df = pd.read_csv('./bird_tracking.csv')
df.to_sql('bird_tracking', conn, if_exists='replace', index=False)
conn.close()
```

```sql
-- 날짜 기반 월별 집계 (substr + group by — SQLite 핵심 패턴)
SELECT device_info_serial 장치,
       substr(date_time, 1, 7) 년월,    -- '2013-08'
       count(*) 건수
FROM bird_tracking
GROUP BY 장치, 년월
ORDER BY 장치, 년월;

-- 기간 파악 / 고유 값 개수
SELECT min(date_time), max(date_time) FROM bird_tracking;
SELECT count(*) 총건수, count(distinct device_info_serial) 장치수 FROM bird_tracking;
```

- `rowid` = SQLite가 모든 테이블에 부여하는 내부 행 번호 (INTEGER PRIMARY KEY와 동일 동작, 삭제 시 재사용 주의)
- `GROUP BY`에 SELECT 별명을 바로 쓰는 건 SQLite 확장 기능 (표준 SQL에선 안 될 수 있음)

---

## 14. SQL 실행 순서

작성 순서와 실행 순서가 다름:

```
FROM → WHERE → GROUP BY → HAVING → SELECT → ORDER BY
```

- 그래서 WHERE에서는 SELECT의 별명을 못 쓴다 (SELECT가 나중 실행)
