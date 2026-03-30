<!DOCTYPE html>
<html lang="es">

  <head>
    <meta charset = "utf-8"/>
    <meta keywords = "Proyecto de integracion Ingenieria en Computación"/>
    <meta description = "Proyecto de integracion Ingenieria en Computación"/>
    <title>Proyecto de integracion Ingenieria en Compución</title>
    <link rel="stylesheet" href="css/general.css">
    <link rel="stylesheet" href="css/printdata.css">
    <meta name="viewport" content="width=device-width, initial-scale=5.0" />
  </head>
  
  <body>
<!-- php include here ---->
<?php
$con=mysqli_connect("localhost","ctrl_inv","password","control_inv");
// Check connection
if (mysqli_connect_errno())
{
echo "Failed to connect to MySQL: " . mysqli_connect_error();
}



$result = mysqli_query($con,"SELECT * FROM tLogLector WHERE ph!='NULL'");


echo  "<div id='cuerpo'><table border='1' style='display: inline-block;'>
<tr>
<th>.Fecha / Hora.</th>
<th>.pH.</th>
</tr>";

while($row = mysqli_fetch_array($result))
{
echo "<tr>";
echo "<td>" . $row['fecha'] . "</td>";
echo "<td>" . $row['ph'] . "</td>";
echo "</tr>";
}
echo "</table>";



$result = mysqli_query($con,"SELECT * FROM tLogLector WHERE ec!='NULL'");

echo "<table border='1' style='display: inline-block;'>
<tr>
<th>.EC.</th>
</tr>";

while($row = mysqli_fetch_array($result))
{
echo "<tr>";
echo "<td>" . $row['ec'] . "</td>";
echo "</tr>";
}
echo "</table>";




$result = mysqli_query($con,"SELECT * FROM tLogLector WHERE temp!='NULL'");

echo "<table border='1'style='display: inline-block;'>
<tr>
<th>Temperatura</th>
<th>Humedad(%)</th>
</tr>";

while($row = mysqli_fetch_array($result))
{
echo "<tr>";
echo "<td>" . $row['temp'] . "</td>";
echo "<td>" . $row['humedad'] . "</td>";
echo "</tr>";
}
echo "</table></div>";


$result = mysqli_query($con,"SELECT * FROM tAlarmas ");

echo "<div id='alarmas'><table border='1' style='text-align:left;'>
<tr>
<th>ALARMAS FECHA</th>
<th>idSensor</th>
<th>Valor</th>
<th>Severidad</th>
</tr>";

while($row = mysqli_fetch_array($result))
{
echo "<tr>";
echo "<td>" . $row['fechaInicio'] . "</td>";
echo "<td>" . $row['idLector'] . "</td>";
echo "<td>" . $row['valor'] . "</td>";
echo "<td style ='color:#'>" . $row['severidad'] . "</td>";
echo "</tr>";
}
echo "</table></div>";

//echo "</section>";

mysqli_close($con);
?>

</body>
</html>