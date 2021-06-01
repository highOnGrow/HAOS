/* Simple C program that connects to MySQL Database server*/
#include <mysql.h>
#include <stdio.h>
void main() {
   MYSQL *conn;
   MYSQL_RES *res;
   MYSQL_ROW row;
   char *server = "localhost";
   char *user = "ctrl_inv";
   char *password = "password"; /* set me first */
   char *database = "control_inv";
   char *q1 = "INSERT INTO cHortaliza VALUES (NULL, 'primerodeC',6,6,6,6,0)";
   char *q2 = "SELECT * FROM cHortaliza WHERE nombre=jitomate"
   conn = mysql_init(NULL);
   /* Connect to database */
   if (!mysql_real_connect(conn, server,
         user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }
   /* send SQL query */


//"INSERT INTO Cars VALUES(8,'Volkswagen',21600)"
   if (mysql_query(conn, q2))
   {
   //if (mysql_query(conn, "show tables")) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }


   res = mysql_use_result(conn);
   /* output table name */
   printf("MySQL Tables in mysql database:\n");
   while ((row = mysql_fetch_row(res)) != NULL)
      printf("%s \n", row[0]);
   /* close connection */
   mysql_free_result(res);
   mysql_close(conn);
}