
PRAGMA foreign_keys = ON;

BEGIN TRANSACTION;

CREATE TABLE IF NOT EXISTS Departments (
    department_id INTEGER PRIMARY KEY AUTOINCREMENT,
    department_name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS Users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    name TEXT NOT NULL,
    email TEXT NOT NULL UNIQUE,
    role TEXT NOT NULL CHECK (
        role IN (
            'Student',
            'Lecturer',
            'HeadOfDepartment',
            'SystemAdministrator'
        )
    ),
    department_id INTEGER,
    FOREIGN KEY (department_id)
        REFERENCES Departments(department_id)
);

CREATE TABLE IF NOT EXISTS Courses (
    course_id INTEGER PRIMARY KEY AUTOINCREMENT,
    course_code TEXT NOT NULL UNIQUE,
    course_name TEXT NOT NULL,
    department_id INTEGER NOT NULL,
    FOREIGN KEY (department_id)
        REFERENCES Departments(department_id)
);

CREATE TABLE IF NOT EXISTS CourseEnrolments (
    enrolment_id INTEGER PRIMARY KEY AUTOINCREMENT,
    student_id INTEGER NOT NULL,
    course_id INTEGER NOT NULL,
    enrolled_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE (student_id, course_id),
    FOREIGN KEY (student_id) REFERENCES Users(user_id),
    FOREIGN KEY (course_id) REFERENCES Courses(course_id)
);

CREATE TABLE IF NOT EXISTS CourseLecturers (
    course_lecturer_id INTEGER PRIMARY KEY AUTOINCREMENT,
    lecturer_id INTEGER NOT NULL,
    course_id INTEGER NOT NULL,
    assigned_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE (lecturer_id, course_id),
    FOREIGN KEY (lecturer_id) REFERENCES Users(user_id),
    FOREIGN KEY (course_id) REFERENCES Courses(course_id)
);

CREATE TABLE IF NOT EXISTS Assignments (
    assignment_id INTEGER PRIMARY KEY AUTOINCREMENT,
    course_id INTEGER NOT NULL,
    lecturer_id INTEGER NOT NULL,
    title TEXT NOT NULL,
    description TEXT,
    deadline TEXT NOT NULL,
    max_mark REAL NOT NULL CHECK (max_mark > 0),
    resubmission_limit INTEGER NOT NULL DEFAULT 0
        CHECK (resubmission_limit >= 0),
    FOREIGN KEY (course_id) REFERENCES Courses(course_id),
    FOREIGN KEY (lecturer_id) REFERENCES Users(user_id)
);

CREATE TABLE IF NOT EXISTS Extensions (
    extension_id INTEGER PRIMARY KEY AUTOINCREMENT,
    assignment_id INTEGER NOT NULL,
    student_id INTEGER NOT NULL,
    revised_deadline TEXT NOT NULL,
    reason TEXT NOT NULL,
    approved INTEGER NOT NULL CHECK (approved IN (0, 1)),
    approved_by INTEGER,
    approved_at TEXT,
    UNIQUE (assignment_id, student_id),
    FOREIGN KEY (assignment_id)
        REFERENCES Assignments(assignment_id),
    FOREIGN KEY (student_id) REFERENCES Users(user_id),
    FOREIGN KEY (approved_by) REFERENCES Users(user_id)
);

CREATE TABLE IF NOT EXISTS Submissions (
    submission_id INTEGER PRIMARY KEY AUTOINCREMENT,
    assignment_id INTEGER NOT NULL,
    student_id INTEGER NOT NULL,
    attempt_no INTEGER NOT NULL CHECK (attempt_no >= 1),
    submitted_at TEXT NOT NULL,
    submission_content TEXT NOT NULL,
    status TEXT NOT NULL CHECK (
        status IN (
            'Submitted',
            'Late',
            'Marked',
            'Moderated',
            'Released'
        )
    ),
    UNIQUE (assignment_id, student_id, attempt_no),
    FOREIGN KEY (assignment_id)
        REFERENCES Assignments(assignment_id),
    FOREIGN KEY (student_id) REFERENCES Users(user_id)
);

CREATE TABLE IF NOT EXISTS Marks (
    mark_id INTEGER PRIMARY KEY AUTOINCREMENT,
    submission_id INTEGER NOT NULL UNIQUE,
    score REAL CHECK (score >= 0),
    feedback TEXT,
    moderation_status TEXT NOT NULL DEFAULT 'Pending'
        CHECK (
            moderation_status IN (
                'Pending',
                'Approved',
                'Rejected'
            )
        ),
    marked_by INTEGER,
    marked_at TEXT,
    moderated_by INTEGER,
    moderated_at TEXT,
    released_at TEXT,
    FOREIGN KEY (submission_id)
        REFERENCES Submissions(submission_id),
    FOREIGN KEY (marked_by) REFERENCES Users(user_id),
    FOREIGN KEY (moderated_by) REFERENCES Users(user_id)
);

CREATE TABLE IF NOT EXISTS AuditRecords (
    audit_id INTEGER PRIMARY KEY AUTOINCREMENT,
    mark_id INTEGER NOT NULL,
    changed_by INTEGER NOT NULL,
    changed_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    old_value REAL,
    new_value REAL NOT NULL CHECK (new_value >= 0),
    reason TEXT NOT NULL,
    FOREIGN KEY (mark_id) REFERENCES Marks(mark_id),
    FOREIGN KEY (changed_by) REFERENCES Users(user_id)
);

CREATE INDEX IF NOT EXISTS idx_courses_department
    ON Courses(department_id);

CREATE INDEX IF NOT EXISTS idx_assignments_course
    ON Assignments(course_id);

CREATE INDEX IF NOT EXISTS idx_submissions_student_assignment
    ON Submissions(student_id, assignment_id);

CREATE INDEX IF NOT EXISTS idx_marks_submission
    ON Marks(submission_id);

CREATE INDEX IF NOT EXISTS idx_audit_mark
    ON AuditRecords(mark_id);

COMMIT;
