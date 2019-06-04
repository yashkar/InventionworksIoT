<?php
$servername = "localhost";
$username = "root";
$password = "root";
$dbname = "";

// Create connection
$conn = new mysqli($servername, $username, $password);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
else{
	echo "Connected successfully <br>";
}

// Create database
$sql = "CREATE DATABASE iot1";

if ($conn->query($sql) === TRUE) {
    echo "Database created successfully <br>";
} else {
    echo "Error creating database: " . $conn->error;
}

echo "<br>";
$conn->close();
$conn1 = new mysqli($servername, $username, $password,'iot1');
if(! $conn1 ) {
      die('Could not connect: ' . mysql_error());
   }
   
echo "<br>";
// sql to create table
$tab = "CREATE TABLE pht (
id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
pressure VARCHAR(30) NOT NULL,
temp VARCHAR(30) NOT NULL,
humidity VARCHAR(50),
reg_date TIMESTAMP
)";

if ($conn1->query($tab) === TRUE) {
    echo "Table created successfully \n";
} else {
    echo "Error creating table: " . $conn1->error;
}
echo "<br>";


$p = $_GET['p'];
$h = $_GET['h'];
$t = $_GET['t'];
echo $p;
echo $h;
echo $t;
//insert data
$sql = "INSERT INTO pht (pressure, temp, humidity)
VALUES ($p, $t, $h)";

if ($conn1->query($sql) === TRUE) {
    echo "New record created successfully";
} else {
    echo "Error: " . $sql . "<br>" . $conn1->error;
}

?>
