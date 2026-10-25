# High School Learning Management System (Phase 4)

C++17 console application using SQLite for persistent storage and Argon2id for salted password hashes. It is intended as a local/offline academic demonstration, not as a production school deployment.

## Quick start (Windows + MSYS2 UCRT64)

1. Install CMake, Ninja, a C++17 compiler, SQLite development files, and Argon2 development files in the MSYS2 **UCRT64** environment. The source expects `C:\msys64\ucrt64\include` and `C:\msys64\ucrt64\lib`.
2. Open PowerShell in this folder (`Highschool LMS`).
3. Configure and build:

   ```powershell
   cmake -S . -B build -G Ninja
   cmake --build build
   ```

4. Run from this folder (so the schema file is found):

   ```powershell
   .\build\HighschoolLMS.exe
   ```

5. Select `1 Login`. The startup process creates any missing demo role accounts and hashes their passwords. If the supplied database contains known demo accounts with legacy non-Argon2 hashes, it replaces those demo hashes so the demonstration accounts can log in. The packaged database has no placeholder test users. The accounts are:

   | Role | Username | Demo password |
   |---|---|---|
   | System administrator | `admin01` | `Admin@12345` |
   | Lecturer | `lecturer01` | `Teach@12345` |
   | Head of Department | `hod01` | `Moderate@12345` |
   | Student | `student01` | `Student@12345` |

   These are demonstration credentials only. Change/remove them before any real use.

## How the system is structured

- `src/main.cpp` — startup, first-run demo accounts, login loop, role menu, validation/error display. `src/SQLiteDatabase.cpp` loads the schema and manages the connection.
- `include/User.h`, `src/User.cpp` — abstract `User` base class and common user information.
- `include/UserRoles.h`, `src/UserRoles.cpp` — `Student`, `Lecturer`, `HeadOfDepartment`, `SystemAdministrator`; virtual role/permission methods demonstrate inheritance and polymorphism.
- `include/SQLiteDatabase.h`, `src/SQLiteDatabase.cpp` — opens/closes SQLite and enables foreign keys.
- `include/UserRepository.h`, `src/UserRepository.cpp` — user data-access layer; parameterized create/read/authenticate/update/delete SQL.
- `include/LmsRepository.h`, `src/LmsRepository.cpp` — data-access operations for departments, courses, enrolment, assignments, extensions, submissions, marks, moderation, release, and audit listing.
- `include/PasswordHasher.h`, `src/PasswordHasher.cpp` — Argon2id hashing/verification and cryptographically random salt generation using Windows CNG.
- `HighSchool_LMS_SQLite_Schema.sql` — normalized relational schema, foreign keys, CHECK/UNIQUE constraints, indexes.
- `lms.db` — local SQLite database used by the program. Do not commit real student data.
- `CMakeLists.txt` — build configuration.

## Main user journeys

1. **Administrator:** login → create department/course → create accounts → enrol students → list/verify rows.
2. **Lecturer:** login → create assignment → optionally grant a student an extension → view submissions → mark and enter feedback plus an audit reason.
3. **Head of Department:** login → view marks → approve/reject with a reason → release only approved marks.
4. **Student:** login → view enrolled courses/assignments → submit/resubmit using an assignment ID → see only marks that have been released.

## Database proof for a newly created user

After an administrator creates a user, the console prints the inserted `user_id`. From the project root, run:

```powershell
sqlite3 lms.db "SELECT user_id, username, name, email, role FROM Users ORDER BY user_id DESC LIMIT 5;"
sqlite3 lms.db "SELECT user_id, username, password_hash FROM Users ORDER BY user_id DESC LIMIT 5;"
```

The second query should show an encoded Argon2id hash (starts with `$argon2id$`), not the original password. To check a specific row, replace the ID in `WHERE user_id=...`.

## Important scope notes

- Assignment dates are entered as `YYYY-MM-DD`. The console currently records a submission's text or a file reference; it does not upload/attach files.
- Late penalties are calculated at marking time: subtract 10% of the maximum mark for each late calendar day up to three days; after three days the score is zero. Approved individual extensions replace the normal deadline for status/penalty calculation.
- The current implementation permits one resubmission and checks that the second attempt is on/before the effective deadline. Students must be enrolled in the assignment course; lecturers must be assigned to the course before creating an assignment.
- Student mark display is filtered to released marks only.
- This is a compact Phase 4 console implementation. Authentication is password-hash based; however, role enforcement is currently menu-level rather than a separately authenticated service/API boundary. Review this design limitation in a production hardening pass.


## Demo administrator display name
The seeded administrator's display name is **Tlholo Victor** (username `admin01`). The demonstration username/password remain `admin01` / `Admin@12345`. Existing databases are not overwritten by seed logic; if `admin01` already exists, use the one-time SQL update described in `CRUD_AND_DEMO_GUIDE.txt`.
