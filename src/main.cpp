#include <iostream>
#include <limits>
#include <memory>
#include <vector>
#include <stdexcept>
#include <string>
#include "SQLiteDatabase.h"
#include "UserRepository.h"
#include "PasswordHasher.h"
#include "UserRoles.h"
#include "LmsRepository.h"
using namespace std;

static string readLine(const string& prompt) { cout << prompt; string s; getline(cin,s); return s; }
static int readInt(const string& prompt) { string s=readLine(prompt); size_t n=0; try {int v=stoi(s,&n); if(n!=s.size()) throw invalid_argument("x"); return v;} catch(...) {throw invalid_argument("Please enter a whole number.");} }
static double readDouble(const string& prompt) { string s=readLine(prompt); size_t n=0; try {double v=stod(s,&n); if(n!=s.size()) throw invalid_argument("x"); return v;} catch(...) {throw invalid_argument("Please enter a number.");} }
static void seedDemoAccounts(UserRepository& users, LmsRepository& lms) {
    int dept=lms.ensureDemoDepartment();
    users.assignDemoUsersToDepartment(dept);
    struct Seed {const char* u;const char* p;const char* n;const char* e;const char* r;};
    Seed seeds[]={{"admin01","Admin@12345","Tlholo Victor","admin01@school.local","SystemAdministrator"},{"lecturer01","Teach@12345","Mpho (Demo Lecturer)","lecturer01@school.local","Lecturer"},{"hod01","Moderate@12345","Lerato (Demo HoD)","hod01@school.local","HeadOfDepartment"},{"student01","Student@12345","Thabo (Demo Student)","student01@school.local","Student"}};
    bool created=false;
    for(auto& x:seeds) {
        if(!users.usernameExists(x.u)) {
            users.createUser(x.u,PasswordHasher::hashPassword(x.p),x.n,x.e,x.r,dept);
            created=true;
        }
    }
    if(created) cout<<"Missing classroom demo accounts were created (change passwords before real use).\n";
}
static bool allowed(const string& role,const string& wanted) {return role==wanted || role=="SystemAdministrator";}
static void printHelp(){cout<<"\nROLE MENU GUIDE\nAdministrator: manage users, departments, courses, assignments and record listings.\nLecturer: assignments, extensions, view submissions and mark/feedback.\nHoD: review marks, approve/reject and release approved marks.\nStudent: submit/resubmit work and view released marks.\nUse ISO dates YYYY-MM-DD (for example 2026-11-01).\n";}
static void appMenu(UserRepository& users,LmsRepository& repo,int uid,const string& name,const string& email,const string& role){
    cout<<"\nWelcome, "<<name<<" ("<<role<<")\n";
    bool running=true;
    while(running){
        cout<<"\n========== HIGH SCHOOL LMS ==========\n";
        if(allowed(role,"SystemAdministrator")) cout<<"1 Create user  2 List users  3 Update user email  4 Delete user\n5 Create department  6 Create course\n";
        if(role=="Lecturer") cout<<"7 Create assignment  8 Grant extension  9 List submissions  10 Mark submission\n";
        if(allowed(role,"Student")) cout<<"11 Submit/resubmit assignment  12 View released marks  23 View my courses/assignments\n";
        if(allowed(role,"HeadOfDepartment")) cout<<"13 List marks  14 Moderate mark  15 Release approved mark\n";
        cout<<"16 List departments  17 List courses  18 List assignments  19 List audit records  20 My profile\n";
        if(allowed(role,"SystemAdministrator")) cout<<"21 Enrol student in course  22 Assign lecturer to course\n";
        cout<<"0 Logout\n";
        try {
            int c=readInt("Choose: ");
            if(c==0){running=false;continue;}
            if(c==1 && allowed(role,"SystemAdministrator")){
                string u=readLine("Username: "),p=readLine("New password (not stored in plain text): "),n=readLine("Full name: "),e=readLine("Email: ");
                cout<<"Role: 1 Student, 2 Lecturer, 3 HeadOfDepartment, 4 SystemAdministrator\n";int r=readInt("Choose role: ");string rs=(r==1?"Student":r==2?"Lecturer":r==3?"HeadOfDepartment":r==4?"SystemAdministrator":""); if(rs.empty())throw invalid_argument("Invalid role choice.");
                int dept=readInt("Department ID (0 if not applicable; use option 16 to see IDs): ");
                if(users.usernameExists(u)) throw runtime_error("Username already exists.");
                int id=users.createUser(u,PasswordHasher::hashPassword(p),n,e,rs,dept);
                cout<<"CREATED: user_id="<<id<<". Verify in SQLite with: SELECT user_id,username,name,role FROM Users WHERE user_id="<<id<<";\n";
            } else if(c==2 && allowed(role,"SystemAdministrator")){users.listUsers();}
            else if(c==3 && allowed(role,"SystemAdministrator")){int id=readInt("User ID: ");string e=readLine("New email: ");users.updateUserEmail(id,e);cout<<"Email updated.\n";}
            else if(c==4 && allowed(role,"SystemAdministrator")){int id=readInt("User ID to delete: ");string confirm=readLine("Type DELETE to confirm: ");if(confirm=="DELETE"){users.deleteUser(id);cout<<"User deleted.\n";}else cout<<"Cancelled.\n";}
            else if(c==5 && allowed(role,"SystemAdministrator")){int id=repo.createDepartment(readLine("Department name: "));cout<<"Department created, ID "<<id<<".\n";}
            else if(c==6 && allowed(role,"SystemAdministrator")){string code=readLine("Course code: ");string cn=readLine("Course name: ");int dept=readInt("Department ID (see option 16): ");int id=repo.createCourse(code,cn,dept);cout<<"Course created, ID "<<id<<".\n";}
            else if(c==7 && role=="Lecturer"){int course=readInt("Course ID: ");string title=readLine("Assignment title: ");string deadline=readLine("Deadline YYYY-MM-DD: ");double max=readDouble("Maximum mark: ");int id=repo.createAssignment(course,uid,title,deadline,max);cout<<"Assignment created, ID "<<id<<".\n";}
            else if(c==8 && allowed(role,"Lecturer")){int a=readInt("Assignment ID: ");int st=readInt("Student user ID: ");string d=readLine("Revised deadline YYYY-MM-DD: ");string why=readLine("Reason for extension: ");repo.grantExtension(a,st,d,why,uid);cout<<"Extension recorded.\n";}
            else if(c==9 && allowed(role,"Lecturer")){repo.listTable("Submissions");}
            else if(c==10 && allowed(role,"Lecturer")){int sid=readInt("Submission ID: ");double score=readDouble("Score: ");string fb=readLine("Feedback: "),reason=readLine("Reason for mark/change (audit): ");repo.markSubmission(sid,uid,score,fb,reason);cout<<"Mark saved and audit record created. Status is Pending HoD moderation.\n";}
            else if(c==11 && allowed(role,"Student")){int a=readInt("Assignment ID: ");string content=readLine("Submission text/file reference: ");string at=readLine("Submitted at YYYY-MM-DD: ");int sid=repo.submitAssignment(a,uid,content,at);cout<<"Submission saved with ID "<<sid<<".\n";}
            else if(c==12 && allowed(role,"Student")){cout<<"Released marks only (unapproved marks remain hidden).\n";repo.listReleasedMarksForStudent(uid);}
            else if(c==13 && allowed(role,"HeadOfDepartment")){repo.listDepartmentMarks(uid);}
            else if(c==14 && allowed(role,"HeadOfDepartment")){int id=readInt("Mark ID: ");string v=readLine("Approve? enter A to approve or R to reject: ");if(v!="A"&&v!="R")throw invalid_argument("Enter A or R.");string reason=readLine("Moderation reason: ");repo.moderateMark(id,uid,v=="A",reason);cout<<"Moderation decision saved.\n";}
            else if(c==15 && allowed(role,"HeadOfDepartment")){repo.releaseMark(readInt("Approved mark ID: "),uid);cout<<"Mark released to student view.\n";}
            else if(c==16){repo.listTable("Departments");} else if(c==17){repo.listTable("Courses");} else if(c==18){repo.listTable("Assignments");} else if(c==19){if(!allowed(role,"HeadOfDepartment"))throw runtime_error("Only HoD/admin may view mark audit records.");repo.listTable("AuditRecords");}
            else if(c==20){cout<<"User ID: "<<uid<<"\nName: "<<name<<"\nEmail: "<<email<<"\nRole: "<<role<<"\n";}
            else if(c==21 && allowed(role,"SystemAdministrator")){int st=readInt("Student user ID: ");int course=readInt("Course ID: ");repo.enrolStudent(st,course);cout<<"Student enrolled.\n";}
            else if(c==22 && allowed(role,"SystemAdministrator")){int lecturer=readInt("Lecturer user ID: ");int course=readInt("Course ID: ");repo.assignLecturer(lecturer,course);cout<<"Lecturer assigned to course.\n";}
            else if(c==23 && allowed(role,"Student")){repo.listStudentCoursesAndAssignments(uid);}
            else {cout<<"That option is not available for your role or is invalid.\n";}
        } catch(const exception& e){cerr<<"Action could not be completed: "<<e.what()<<"\n";}
    }
}
int main(){
    try {
        SQLiteDatabase database("lms.db"); database.initializeSchema("HighSchool_LMS_SQLite_Schema.sql");
        UserRepository users(database); LmsRepository repo(database); seedDemoAccounts(users,repo); users.migrateLegacyDemoPasswords();
        cout<<"High School LMS | SQLite connected | offline console mode\n";
        bool run=true;while(run){cout<<"\n1 Login\n2 Help / user journey\n0 Exit\n";try{int choice=readInt("Choose: ");if(choice==0)run=false;else if(choice==2)printHelp();else if(choice==1){string u=readLine("Username: "),p=readLine("Password: ");int id=0;string n,e,r;if(users.authenticate(u,p,id,n,e,r)){cout<<"Login successful.\n";appMenu(users,repo,id,n,e,r);}else cout<<"Login failed. Check credentials.\n";}else cout<<"Choose 0, 1 or 2.\n";}catch(const exception&e){cerr<<"Input error: "<<e.what()<<"\n";}}
    } catch(const exception& e){cerr<<"Fatal startup/database error: "<<e.what()<<"\n";return 1;} return 0;
}
