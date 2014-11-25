<?php

parse_str(implode('&', array_slice($argv, 1)), $_POST);

$error = '';
if( isset($_POST['username']) && !empty($_POST['username'])){
	$username = $_POST['username'];
	if($username == "username"){

		if( isset($_POST['password']) && !empty($_POST['password'])){
			$password = $_POST['password'];
			if($password == "password"){
				//echo "You have successfully logged in.</br>";
			} else {
				$error = "username/password is incorrect.</br>";
			}
		} else {
			$error = "No password was entered.</br>";
		}

	} else {
		$error = "username/password is incorrect.</br>";
	}

} else {
	$error = "No username was set.</br>";
}

?>
<!DOCTYPE html>
<html>
<head>
	<title>Validation Page</title>
</head>
<body>
<?php

if(strlen($error) > 0){
	echo $error;
	echo "<p>Redirecting to login in 5 seconds.</p>";
	echo '<script> setTimeout(function(){ window.location="form.html"; }, 5000); </script>';
} else {
	echo "<p>You have been successfully logged in.</p>";
}

?>	
</body>
</html>