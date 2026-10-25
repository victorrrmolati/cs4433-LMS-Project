#ifndef LMS_REPOSITORY_H
#define LMS_REPOSITORY_H
#include "SQLiteDatabase.h"
#include <string>
class LmsRepository {
    SQLiteDatabase& database;
public:
    explicit LmsRepository(SQLiteDatabase& db): database(db) {}
    int createDepartment(const std::string& name);
    int ensureDemoDepartment();
    int createCourse(const std::string& code,const std::string& name,int departmentId);
    int createAssignment(int courseId,int lecturerId,const std::string& title,const std::string& deadline,double maxMark);
    void enrolStudent(int studentId,int courseId);
    void assignLecturer(int lecturerId,int courseId);
    void listStudentCoursesAndAssignments(int studentId);
    int submitAssignment(int assignmentId,int studentId,const std::string& content,const std::string& submittedAt);
    void listTable(const std::string& table);
    void listReleasedMarksForStudent(int studentId);
    void listDepartmentMarks(int hodId);
    void markSubmission(int submissionId,int lecturerId,double score,const std::string& feedback,const std::string& reason);
    void moderateMark(int markId,int hodId,bool approve,const std::string& reason);
    void releaseMark(int markId,int hodId);
    void grantExtension(int assignmentId,int studentId,const std::string& revisedDeadline,const std::string& reason,int approvedBy);
};
#endif
