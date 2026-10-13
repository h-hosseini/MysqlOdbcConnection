# Qt, MySQL, and ODBC Setup Guide

A step-by-step guide to connecting a C++ application built with Qt 6.6 and MinGW 64-bit to MySQL using the Qt SQL module and MySQL Connector/ODBC.

This guide also includes a small utility for listing available Qt SQL drivers and troubleshooting connection issues.

## Table of Contents

- [Overview](#overview)
- [Tested Environment](#tested-environment)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
- [1. Enable Qt SQL in CMake](#1-enable-qt-sql-in-cmake)
- [2. Check Available Qt SQL Drivers](#2-check-available-qt-sql-drivers)
- [3. Install MySQL Connector/ODBC](#3-install-mysql-connectorodbc)
- [4. Create the MySQL Database and User](#4-create-the-mysql-database-and-user)
- [5. Connect to MySQL Using QODBC](#5-connect-to-mysql-using-qodbc)
- [6. Common Errors and Solutions](#6-common-errors-and-solutions)
- [راهنمای فارسی](#فارسی)
## Overview

Qt SQL supports multiple database drivers. The `QODBC` driver allows a Qt application to access databases through an installed ODBC driver.

In this setup, the application uses:

- Qt SQL's `QODBC` driver.
- MySQL Connector/ODBC.
- A MySQL Server instance.

**Important:** Having `QODBC` available in Qt does not mean that the MySQL ODBC driver is installed. Both components are required.

## Tested Environment

The following configuration was used for the connection test:

| Component | Version / Configuration |
|---|---|
| Operating system | Windows |
| Qt | 6.6.0 |
| Compiler | MinGW 64-bit |
| Build system | CMake |
| MySQL Server | 9.1.0 |
| MySQL Connector/ODBC | 26.7.1, 64-bit |
| ODBC driver | MySQL ODBC Unicode Driver |
| Qt SQL driver | QODBC |

Your configuration may differ. The ODBC driver name shown in Windows must match the name used in the connection string.

## Requirements

- Qt 6.6 or a compatible Qt 6 release.
- A C++ compiler supported by the selected Qt kit.
- CMake.
- The Qt SQL module.
- MySQL Server.
- MySQL Connector/ODBC for Windows, 64-bit.

Downloads:

- [Qt](https://www.qt.io/download)
- [MySQL Connector/ODBC](https://dev.mysql.com/downloads/connector/odbc/)
- [MySQL Server](https://dev.mysql.com/downloads/mysql/)

## Getting Started

You can try this project in either of the following ways.

### Option 1: Clone or Download the Repository

1. Clone the repository using Git:

   ```bash
   git clone https://github.com/h-hosseini/MysqlOdbcConnection.git
   ```

   Alternatively, download the project as a ZIP file from GitHub and extract it.

2. Open the project in Qt Creator by selecting the `CMakeLists.txt` file.

3. Select a compatible Qt Kit, such as **Qt 6.6.0 MinGW 64-bit**.

4. Configure and build the project.

5. Run the application to see the available SQL drivers and check the database driver configuration.

### Option 2: Create a New Console Application

If you want to learn how the code works or test it in your own environment, you can create a new project in Qt Creator.

1. Open Qt Creator and select **File → New Project**.
2. Choose **Application (Qt) → Qt Console Application**. The exact menu names may vary by Qt Creator version.
3. Enter a project name and choose a location.
4. Select a CMake as the build system.
5. Select a compatible Qt Kit, such as **Qt 6.6.0 MinGW 64-bit**.
6. Configure the project to use the **Core** and **Sql** modules.
7. Copy the source code from this repository into your new project's source file, such as `main.cpp`.
8. Build and run the application.

Make sure your `CMakeLists.txt` includes the Qt SQL module and links against it:

```cmake
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

target_link_libraries(YourProjectName
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
)
```

Replace `YourProjectName` with the actual target name defined in your CMake project. In this case, we use **MysqlOdbcConnection**.

**Note:** If you only want to list the available SQL drivers, you do not need a running MySQL server. To test a MySQL connection, you must also configure MySQL and install a compatible database driver, such as MySQL Connector/ODBC when using `QODBC`.

## 1. Enable Qt SQL in CMake

Add the `Sql` component to `find_package()` and link the Qt SQL library.

Example `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)

project(MysqlOdbcConnection LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED
    COMPONENTS Core Sql
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED
    COMPONENTS Core Sql
)

add_executable(MysqlOdbcConnection
    main.cpp
)

target_link_libraries(MysqlOdbcConnection
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
)
```

After modifying the CMake file, reconfigure and rebuild the project in Qt Creator.

## 2. Check Available Qt SQL Drivers

Use the following code to list the SQL drivers available to your application.

```cpp
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "Qt SQL drivers:"
             << QSqlDatabase::drivers();

    qDebug() << "Plugin paths:"
             << QCoreApplication::libraryPaths();

    if (!QSqlDatabase::drivers().contains("QODBC")) {
        qCritical() << "QODBC driver is missing.";
        return 1;
    }

    qDebug() << "QODBC driver is available.";
    return 0;
}
```

If `QODBC` appears in the output, Qt has found its ODBC plugin.

This does not yet confirm that the MySQL ODBC driver or its dependencies can be loaded.

## 3. Install MySQL Connector/ODBC

1. Open the official [MySQL Connector/ODBC download page](https://dev.mysql.com/downloads/connector/odbc/).
2. Download the Windows 64-bit MSI installer.
3. Install the connector.

After installing MySQL Connector/ODBC, verify the installed driver names:

1. Open the Windows Start menu.
2. Search for **ODBC Data Sources (64-bit)** and open it.
3. Select the **Drivers** tab.
4. Find the installed **MySQL ODBC** drivers. Depending on the installation, you may see one or both of the following:
   - `MySQL ODBC 26.7 ANSI Driver`
   - `MySQL ODBC 26.7 Unicode Driver`
5. Copy the exact driver name displayed on your system and use it in the Qt connection string.

**ANSI vs. Unicode**

- **ANSI Driver:** Uses an ANSI-oriented character interface. Character conversion may depend on the Windows code page and application configuration, which can complicate handling Persian text.
- **Unicode Driver:** Supports Unicode character handling and is generally the recommended option for applications that store Persian, Arabic, English, or other multilingual text.

For this project, **prefer the Unicode driver**, especially for student names, family names, addresses, and other Persian-language data. The actual driver name may differ depending on the installed Connector/ODBC version.

Example using the Unicode driver:

```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

db.setDatabaseName(
    "DRIVER={MySQL ODBC 26.7 Unicode Driver};"
    "SERVER=127.0.0.1;"
    "PORT=3306;"
    "DATABASE=school_db;"
    "UID=school_app;"
    "PWD=YOUR_PASSWORD;"
);
```

If only the ANSI driver is installed, replace the driver name with the exact name shown in the **ODBC Data Sources (64-bit)** application.

**Important:** Use the 64-bit ODBC administrator when working with a 64-bit Qt application. Also, configure the database and its tables to use `utf8mb4` so that MySQL can store Unicode text correctly.

## 4. Create the MySQL Database and User

Connect to MySQL using an administrative account and run:

```sql
CREATE DATABASE school_db
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;
```

Create a dedicated application user for a local connection test:

```sql
CREATE USER 'school_app'@'localhost'
    IDENTIFIED BY 'ReplaceWithYourOwnStrongPassword';
```

Grant the permissions needed for this example:

```sql
GRANT SELECT, INSERT, UPDATE, DELETE,
      CREATE, ALTER, INDEX
ON school_db.*
TO 'school_app'@'localhost';
```

Check the grants:

```sql
SHOW GRANTS FOR 'school_app'@'localhost';
```

These commands assume that the user does not already exist. If the account already exists, inspect its grants before changing them.

Use a strong password of your own. Do not commit real credentials to GitHub.

## 5. Connect to MySQL Using QODBC

The following example connects to a local MySQL server and retrieves its version.

```cpp
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName(
        "DRIVER={MySQL ODBC 26.7 Unicode Driver};"
        "SERVER=127.0.0.1;"
        "PORT=3306;"
        "DATABASE=school_db;"
        "UID=school_app;"
        "PWD=ReplaceWithYourOwnStrongPassword;"
    );

    if (!db.open()) {
        qCritical() << "Connection failed:"
                    << db.lastError().text();
        return 1;
    }

    QSqlQuery query(db);

    if (!query.exec("SELECT VERSION()")) {
        qCritical() << "Query failed:"
                    << query.lastError().text();
        return 2;
    }

    if (query.next()) {
        qDebug() << "Connected to MySQL:"
                 << query.value(0).toString();
    }

    db.close();
    return 0;
}
```

Replace the driver name, username, and password with your actual settings.

**Security note:** This example uses hard-coded credentials for simplicity. For a real application, load credentials from a protected configuration mechanism and restrict database privileges.

Expected successful output:

```text
Connected to MySQL: "9.1.0"
```

The server version shown here is an example of the tested environment, not a fixed requirement.

## 6. Common Errors and Solutions

### `QSqlDatabase: No such file or directory`

**Cause:** The Qt SQL module is not configured for the project.

**Solution:** Add `Sql` to `find_package()` and link `Qt6::Sql` or `Qt${QT_VERSION_MAJOR}::Sql`.

### `QMYSQL driver is missing`

**Cause:** The native Qt MySQL driver is not available.

**Solution:** If using ODBC, check for `QODBC` instead. `QMYSQL` and `QODBC` are different Qt SQL drivers.

### `Specified driver could not be loaded (system error 126)`

**Cause:** The ODBC driver or one of its dependent DLLs cannot be loaded.

**Solution:**

- Install the official Windows 64-bit MSI package.
- Check that the driver and its dependencies are 64-bit.
- Check required runtime libraries, such as the Microsoft Visual C++ Redistributable.
- Verify the driver in **ODBC Data Sources (64-bit)**.

### `Access denied for user`

**Cause:** The MySQL user lacks the required permissions, or its credentials or host configuration are incorrect.

**Solution:** Check the account and its grants using an administrative MySQL session.

### `Unknown database 'school_db'`

**Cause:** The database does not exist or the configured name is incorrect.

**Solution:** Create the database or correct the database name in the connection string.

### Connection works locally but fails from another computer

**Possible causes:**

- MySQL is listening only on the local interface.
- The firewall blocks the MySQL port.
- The MySQL account is restricted to `localhost`.
- The network or server configuration does not permit remote connections.

For a school LAN, configure access only for trusted computers or the required local subnet. Do not expose the MySQL port directly to the public internet.

## فارسی

# راهنمای راه‌اندازی اتصال Qt به MySQL از طریق ODBC

این راهنما نحوه اتصال یک برنامه C++ مبتنی بر Qt 6.6 و MinGW 64-bit به MySQL را با استفاده از ماژول Qt SQL و درایور MySQL Connector/ODBC توضیح می‌دهد.

## محیط آزمایش‌شده

| مؤلفه | نسخه یا تنظیمات |
|---|---|
| سیستم‌عامل | Windows |
| Qt | 6.6.0 |
| کامپایلر | MinGW 64-bit |
| سیستم ساخت | CMake |
| MySQL Server | 9.1.0 |
| MySQL Connector/ODBC | 26.7.1، نسخه ۶۴ بیتی |
| درایور ODBC | MySQL ODBC Unicode Driver |
| درایور Qt SQL | QODBC |

این نسخه‌ها مشخصات محیط آزمایش هستند و الزاماً تنها ترکیب قابل استفاده نیستند.

## پیش‌نیازها

- نصب Qt 6.6 یا نسخه سازگار.
- نصب کامپایلر مناسب کیت Qt.
- نصب CMake.
- نصب ماژول Qt SQL.
- نصب MySQL Server.
- نصب MySQL Connector/ODBC نسخه ۶۴ بیتی.

لینک‌های رسمی:

- [دانلود Qt](https://www.qt.io/download)
- [دانلود MySQL Connector/ODBC](https://dev.mysql.com/downloads/connector/odbc/)
- [دانلود MySQL Server](https://dev.mysql.com/downloads/mysql/)

## شروع کار

برای آزمایش و اجرای این پروژه، می‌توانی از یکی از دو روش زیر استفاده کنی.

### روش اول: دریافت یا کلون کردن مخزن پروژه

۱. با استفاده از Git، مخزن پروژه را کلون کن:

```bash
git clone https://github.com/h-hosseini/MysqlOdbcConnection.git
```

همچنین می‌توانی از صفحه GitHub پروژه، فایل ZIP را دانلود و از حالت فشرده خارج کنی.

۲. در Qt Creator، فایل `CMakeLists.txt` پروژه را باز کن.

۳. یک Qt Kit سازگار، مانند **Qt 6.6.0 MinGW 64-bit**، انتخاب کن.

۴. مراحل پیکربندی و ساخت پروژه را انجام بده.

۵. برنامه را اجرا کن تا درایورهای SQL موجود را مشاهده کنی و تنظیمات درایور پایگاه داده را بررسی کنی.

### روش دوم: ساخت یک پروژه کنسولی جدید

## پروژه کنسولی جدید

اگر می‌خواهی کدها را شخصاً وارد کنی، با ساختار برنامه آشنا شوی یا آن را در محیط خودت آزمایش کنی، می‌توانی یک پروژه جدید در برنامه‌ی Qt Creator بسازی.

۱. برنامه‌ی Qt Creator را باز کن و از منوی **File → New Project** وارد بخش ساخت پروژه جدید شو.

۲. گزینه **Application (Qt) → Qt Console Application** را انتخاب کن. نام دقیق گزینه‌ها ممکن است با توجه به نسخه Qt Creator متفاوت باشد.

۳. نام پروژه و مسیر ذخیره‌سازی آن را مشخص کن.

۴. گزینه **CMake** را به‌عنوان Build System انتخاب کن.

۵. یک Qt Kit سازگار با عنوان **Desktop Qt 6.6.0 MinGW 64-bit** انتخاب کن.

۶. ماژول‌های **Core** و **Sql** را برای پروژه در نظر بگیر.

۷. کد منبع موجود در این مخزن را در فایل متناظر پروژه جدید، مانند `main.cpp`، کپی کن.

۸. پروژه را Build و سپس اجرا کن.
مطمئن شو فایل `CMakeLists.txt` ماژول SQL را پیدا کرده و به پروژه متصل می‌کند:

```cmake
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

target_link_libraries(YourProjectName
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
)
```

عبارت `YourProjectName` را با نام واقعی Target پروژه خودت جایگزین کن(در پروژه‌ی ما MysqlOdbcConnection).

**نکته:** اگر فقط می‌خواهی فهرست درایورهای SQL موجود را مشاهده کنی، نیازی به اجرای سرور MySQL نداری. اما برای آزمایش اتصال به MySQL، باید سرور پایگاه داده را تنظیم کنی و درایور سازگار آن، مانند MySQL Connector/ODBC هنگام استفاده از `QODBC`، نصب شده باشد.

## ۱. اضافه کردن Qt SQL به پروژه

در فایل `CMakeLists.txt` باید مؤلفه `Sql` را به `find_package()` اضافه کنیم و کتابخانه مربوط به آن را به پروژه پیوند دهیم.

نمونه:

```cmake
find_package(QT NAMES Qt6 Qt5 REQUIRED
    COMPONENTS Core Sql
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED
    COMPONENTS Core Sql
)

target_link_libraries(MysqlOdbcConnection
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Sql
)
```

پس از تغییر فایل، پروژه را در Qt Creator دوباره پیکربندی و کامپایل کنید.

## ۲. بررسی درایورهای SQL موجود در Qt

با اجرای این کد می‌توان درایورهای قابل‌دسترسی برنامه را مشاهده کرد:

```cpp
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "Qt SQL drivers:"
             << QSqlDatabase::drivers();

    qDebug() << "Plugin paths:"
             << QCoreApplication::libraryPaths();

    return 0;
}
```

اگر `QODBC` در خروجی دیده شود، افزونه ODBC مربوط به Qt در دسترس است. بااین‌حال، این موضوع به‌تنهایی بارگذاری موفق درایور MySQL یا وابستگی‌های آن را تضمین نمی‌کند.

## ۳. نصب MySQL Connector/ODBC

۱. به [صفحه رسمی دانلود MySQL Connector/ODBC](https://dev.mysql.com/downloads/connector/odbc/) مراجعه کنید.

۲. نصب‌کننده MSI ویندوز ۶۴ بیتی را دانلود و اجرا کنید.


پس از نصب MySQL Connector/ODBC، نام درایورهای نصب‌شده را بررسی کنید:

۱. منوی Start ویندوز را باز کنید.

۲. عبارت **ODBC Data Sources (64-bit)** را جست‌وجو و اجرا کنید.

۳. وارد زبانه **Drivers** شوید.

۴. درایورهای MySQL نصب‌شده را پیدا کنید. بسته به نوع نصب، ممکن است یک یا هر دو مورد زیر را ببینیید:

- `MySQL ODBC 26.7 ANSI Driver`
- `MySQL ODBC 26.7 Unicode Driver`

۵. نام دقیق درایور را از سیستم خودتان بردارید و در رشته اتصال Qt قرار دهید. شماره نسخه و نام ثبت‌شده ممکن است در سیستم‌های مختلف متفاوت باشد.

**تفاوت درایورهای ANSI و Unicode**

- **درایور ANSI:** از رابط کاراکتری مبتنی بر ANSI استفاده می‌کند. تبدیل کاراکترها ممکن است به کدپیج ویندوز و تنظیمات برنامه وابسته باشد؛ بنابراین مدیریت متن فارسی با آن می‌تواند پیچیده‌تر شود.
- **درایور Unicode:** برای کار با متن‌های یونیکد طراحی شده و معمولاً انتخاب مناسب‌تری برای برنامه‌هایی است که اطلاعات فارسی، عربی، انگلیسی یا چندزبانه ذخیره می‌کنند.

برای این برنامه، **درایور Unicode را پیشنهاد می‌کنیم**؛ چون قرار است نام و نام خانوادگی، نشانی‌ها و سایر اطلاعات فارسی به‌درستی ذخیره و بازیابی شوند.

نمونه رشته اتصال با درایور Unicode:

```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

db.setDatabaseName(
    "DRIVER={MySQL ODBC 26.7 Unicode Driver};"
    "SERVER=127.0.0.1;"
    "PORT=3306;"
    "DATABASE=school_db;"
    "UID=school_app;"
    "PWD=YOUR_PASSWORD;"
);
```

اگر فقط درایور ANSI نصب شده است، نام آن را دقیقاً مطابق چیزی که در بخش **ODBC Data Sources (64-bit)** نمایش داده می‌شود، جایگزین نام درایور Unicode کنید.

**نکته مهم:** اگر برنامه Qt شما ۶۴ بیتی است، از ابزار مدیریت ODBC نسخه ۶۴ بیتی استفاده کنید. همچنین پایگاه داده و جدول‌های آن باید از مجموعه نویسه `utf8mb4` استفاده کنند تا متن‌های فارسی به‌درستی ذخیره شوند.


## ۴. ساخت دیتابیس و کاربر MySQL

با حساب مدیریتی MySQL وارد شوید و دیتابیس را بسازید:

```sql
CREATE DATABASE school_db
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;
```

سپس یک کاربر اختصاصی برای آزمایش اتصال محلی ایجاد کنید:

```sql
CREATE USER 'school_app'@'localhost'
    IDENTIFIED BY 'ReplaceWithYourOwnStrongPassword';
```

مجوزهای موردنیاز این مثال را بدهید:

```sql
GRANT SELECT, INSERT, UPDATE, DELETE,
      CREATE, ALTER, INDEX
ON school_db.*
TO 'school_app'@'localhost';
```

برای بررسی مجوزها:

```sql
SHOW GRANTS FOR 'school_app'@'localhost';
```

این دستورات فرض می‌کنند کاربر از قبل ساخته نشده است. اگر کاربر وجود دارد، ابتدا مجوزهای فعلی آن را بررسی کنید.

رمز عبور قوی و اختصاصی انتخاب کنید و اطلاعات واقعی اتصال را در مخزن عمومی GitHub قرار ندهید.

## ۵. اتصال Qt به MySQL از طریق QODBC

در کد زیر، نام درایور، نام کاربر و رمز عبور را با مقادیر واقعی سیستم خود جایگزین کنید:

```cpp
QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

db.setDatabaseName(
    "DRIVER={MySQL ODBC 26.7 Unicode Driver};"
    "SERVER=127.0.0.1;"
    "PORT=3306;"
    "DATABASE=school_db;"
    "UID=school_app;"
    "PWD=ReplaceWithYourOwnStrongPassword;"
);

if (!db.open()) {
    qCritical() << "Connection failed:"
                << db.lastError().text();
    return 1;
}

QSqlQuery query(db);

if (query.exec("SELECT VERSION()") && query.next()) {
    qDebug() << "Connected to MySQL:"
             << query.value(0).toString();
}
```

برای استفاده از این قطعه کد، هدرهای `QSqlDatabase`، `QSqlQuery`، `QSqlError` و `QDebug` را نیز اضافه کنید.

خروجی موفق مورد انتظار:

```text
Connected to MySQL: "9.1.0"
```

نسخه نمایش‌داده‌شده مربوط به محیط آزمایش است.

## ۶. خطاهای رایج

### خطای `QSqlDatabase: No such file or directory`

**علت:** ماژول Qt SQL به پروژه اضافه نشده است.

**راه‌حل:** مؤلفه `Sql` را در CMake اضافه کنید و کتابخانه `Qt6::Sql` را پیوند دهید.

### خطای `QMYSQL driver is missing`

**علت:** درایور بومی MySQL برای Qt موجود نیست.

**راه‌حل:** اگر از ODBC استفاده می‌کنید، وجود `QODBC` را بررسی کنید. `QMYSQL` و `QODBC` دو درایور متفاوت هستند.

### خطای `Specified driver could not be loaded (system error 126)`

**علت:** خود درایور ODBC یا یکی از DLLهای وابسته به آن قابل بارگذاری نیست.

**راه‌حل:**

- نصب بسته رسمی MSI نسخه ۶۴ بیتی.
- بررسی ۶۴ بیتی بودن درایور و وابستگی‌های آن.
- بررسی کتابخانه‌های زمان اجرا مانند Microsoft Visual C++ Redistributable.
- بررسی درایور در برنامه `ODBC Data Sources (64-bit)`.

### خطای `Access denied for user`

**علت:** کاربر مجوز کافی ندارد یا اطلاعات ورود و تنظیمات میزبان صحیح نیستند.

**راه‌حل:** حساب کاربری و مجوزهای آن را با حساب مدیریتی MySQL بررسی کنید.

### خطای `Unknown database 'school_db'`

**علت:** دیتابیس وجود ندارد یا نام آن اشتباه است.

**راه‌حل:** دیتابیس را ایجاد کنید یا نام آن را در رشته اتصال اصلاح کنید.

### اتصال محلی موفق است، اما اتصال از رایانه دیگر ناموفق است

علت می‌تواند محدود بودن اتصال MySQL به رابط محلی، مسدود بودن پورت در فایروال یا محدود بودن حساب کاربری به `localhost` باشد.

برای استفاده در شبکه مدرسه، دسترسی را به رایانه‌های مورداعتماد یا شبکه محلی موردنیاز محدود کنید و پورت MySQL را مستقیماً در اینترنت منتشر نکنید.
