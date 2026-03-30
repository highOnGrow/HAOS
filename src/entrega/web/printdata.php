<?php
$con=mysqli_connect("localhost","ctrl_inv","password","control_inv");
// Check connection
if (mysqli_connect_errno())
{
echo "Failed to connect to MySQL: " . mysqli_connect_error();
}

$result = mysqli_query($con,"SELECT * FROM tLogLector WHERE ph!='NULL'");


echo "<table border='1'>
<tr>
<th>Fecha</th>
<th>pH</th>
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

echo "<table border='1'>
<tr>
<th>Fecha</th>
<th>EC</th>
</tr>";

while($row = mysqli_fetch_array($result))
{
echo "<tr>";
echo "<td>" . $row['fecha'] . "</td>";
echo "<td>" . $row['ec'] . "</td>";
echo "</tr>";
}
echo "</table>";




$result = mysqli_query($con,"SELECT * FROM tLogLector WHERE temp!='NULL'");

echo "<table border='1'>
<tr>
<th>Fecha</th>
<th>Tempetura</th>
<th>Humedad(%)</th>
</tr>";

while($row = mysqli_fetch_array($result))
{
echo "<tr>";
echo "<td>" . $row['fecha'] . "</td>";
echo "<td>" . $row['temp'] . "</td>";
echo "<td>" . $row['humedad'] . "</td>";
echo "</tr>";
}
echo "</table>";

//echo "</section>";

mysqli_close($con);
?>
