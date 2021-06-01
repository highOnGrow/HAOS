<?php

$host="localhost"; // Host name
$username="ctrl_inv"; // Mysql username
$password="password"; // Mysql password
$db_name="control_inv"; // Database name
$tbl_name="sUser"; // Table name

// Connect to server and select databse.
mysql_connect("$host", "$username", "$password")or die("cannot connect");
mysql_select_db("$db_name")or die("cannot select DB");

// username and password sent from form
$myusername=$_POST['myusername'];
$mypassword=$_POST['mypassword'];

// To protect MySQL injection (more detail about MySQL injection)
$myusername = stripslashes($myusername);
$mypassword = stripslashes($mypassword);
$myusername = mysql_real_escape_string($myusername);
$mypassword = mysql_real_escape_string($mypassword);


$sql="SELECT * FROM $tbl_name WHERE username='$myusername' and password='$mypassword'";
$result=mysql_query($sql);

// Mysql_num_row is counting table row
$count=mysql_num_rows($result);

if($count==1){
header('Location: http://10.42.0.71/printdata.php');
exit;
}
else {
echo "Usuario o Contraseña Erronea";
}
?>