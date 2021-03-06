#include "staffmember.h"
#include <QString>
#include "course.h"

#include <QSqlDatabase>
#include <QSql>
#include <QSqlError>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QSqlError>
#include <globalDbObject.h>

static QString staff_table = "staff_members";
static QStringList staff_columns = {"first_name","last_name", "degree", "birth_date",
                                "gendre", "address", "picture", "department"};
static QStringList staff_types = {"INTEGER PRIMARY KEY AUTOINCREMENT", "TEXT", "TEXT", "INTEGER", "TEXT",
                              "TEXT", "TEXT", "TEXT", "TEXT PRIMARY KEY", "TEXT"};

StaffMember::StaffMember() : Person() {
    this->degree = QString("");
    this->courses = QVector<Course>();
}

StaffMember::StaffMember(QString first_name, QString last_name, QString gendre, QString picture, QString department, QString address, QString birth_date, QString degree)
    : Person(first_name, last_name, gendre, picture, department, address, birth_date) {
    this->degree = degree;
    this->courses = QVector<Course>();
}

void StaffMember::setDegree(QString degree) {
    this->degree = degree;
}

QString StaffMember::getDegree() {
    return this->degree;
}


QVector<Course> StaffMember::getCourses() {
    return this->courses;
}

void StaffMember::addCourse(QString course_name) {
    this->courses.push_back(Course(course_name));
}

void StaffMember::deleteCourse(QString course_name) {
    int index = -1;
    for(int i = 0; i < this->courses.size(); i++) {
        if(this->courses[0].getName() == course_name) {
            index = i;
            break;
        }
    }
    if(index != -1)
        this->courses.remove(index);
}

QVector<StaffMember> StaffMember::all() {
    QSqlQuery query = SQLiteDb.sql_getQuery();

    QVector<StaffMember> staff_members;
    StaffMember temp;
    query.exec("SELECT * FROM staff_members");
    QSqlQuery query1;
    while(query.next()) {
        long long id = query.value(0).toLongLong();
        QString first_name = query.value(1).toString();
        QString last_name = query.value(2).toString();
        QString gendre = query.value(3).toString();
        QString picture = query.value(4).toString();
        QString department = query.value(5).toString();
        QString birth_date = query.value(6).toString();
        QString address = query.value(7).toString();
        QString degree = query.value(8).toString();

        StaffMember temp(first_name, last_name, gendre, picture, department, birth_date, address, degree);
        temp.setId(id);
        temp.setIsSaved(true);

        query1.exec("SELECT * FROM courses_staff_members WHERE course_id = " + QString::number(id));
        while(query1.next()) {
            temp.addCourse(Course::find(query1.value(1).toLongLong()).getName());
        }
        staff_members.push_back(temp);
    }

    return staff_members;
}

StaffMember StaffMember::find(long long id) {
    QSqlQuery query = SQLiteDb.sql_getQuery();

    query.exec("SELECT * FROM staff_members");
    QSqlQuery query1;
    query.next();
    QString first_name = query.value(1).toString();
    QString last_name = query.value(2).toString();
    QString gendre = query.value(3).toString();
    QString picture = query.value(4).toString();
    QString department = query.value(5).toString();
    QString birth_date = query.value(6).toString();
    QString address = query.value(7).toString();
    QString degree = query.value(8).toString();

    StaffMember staff_member(first_name, last_name, gendre, picture, department, birth_date, address, degree);
    staff_member.setId(id);
    staff_member.setIsSaved(true);

    query1.exec("SELECT * FROM courses_staff_members WHERE course_id = " + QString::number(id));
    while(query1.next()) {
        staff_member.addCourse(Course::find(query1.value(1).toLongLong()).getName());
    }

    return staff_member;
}

bool StaffMember::save(){
    QString id_ = QString::number(getId());
    SQLiteDb.sql_select("*", staff_table, " id = " + id_);
    QSqlQuery query = SQLiteDb.sql_getQuery();
    QStringList values = {getFirstName(),  getLastName(), getDegree(), getBirthDate(),
                          getGendre(), getAddress(), getPicture(),  getDepartment()};
    if(query.next()){
        SQLiteDb.sql_update(staff_table, staff_columns, values, "id = " + id_);

        for(int i = 0; i < courses.size(); i++){
            QString course_id = QString::number(courses[i].getId());
            SQLiteDb.sql_select("*", "courses_staff_members", " staff_member_id = " + id_ + " AND course_id " + course_id);
            query = SQLiteDb.sql_getQuery();
            if(!query.next()){
                SQLiteDb.sql_insert("courses_staff_members", {"staff_member_id", "courses_id"}, {id_, course_id});
            }
        }
        return true;
    }
    SQLiteDb.sql_insert(staff_table, staff_columns, values);
    for(int i = 0; i < courses.size(); i++){
        QString course_id = QString::number(courses[i].getId());
        SQLiteDb.sql_insert("courses_staff_members", {"staff_member_id", "courses_id"}, {id_, course_id});
    }
    return false;
}

void StaffMember::delete1(){
    QString staff_id = QString::number(getId());
    SQLiteDb.sql_delete(staff_table, "id = " + staff_id);
    SQLiteDb.sql_delete("courses_students", "staff_member_id = " + staff_id);
}

QVector<StaffMember> StaffMember::where(QString column, QString value){
    SQLiteDb.sql_select("*", staff_table, column + " = " +  value);
    QSqlQuery query = SQLiteDb.sql_getQuery();
    QVector<StaffMember> staff_members;
    while (query.next()) {
        int id = query.value(0).toInt();
        staff_members.push_back(find(id));
    }
    return staff_members;
}














