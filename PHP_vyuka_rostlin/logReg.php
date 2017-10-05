<!DOCTYPE html>
<html>
<head>
  <title>Mainpage</title>
  <link rel="stylesheet" type="text/css" href="./style.css">
</head>
<body>
  <div class="top-panel">
    <ul>
      <li><a href="index.php" >Domu</a></li>
      <li class="hide"><a href="#" >nic</a></li>
    </ul>
  </div>
  <div class="container">
  <h1>Vitejte</h1>
    <div class="login">
    <form action="login.php" method="post">
      Jmeno:<input id="jmeno" type="text" name="username">
      Heslo:<input type="password" name="password">
            <input type="submit" value="Login">
      <p>Registrace <a href="registration.php">zde</a></p>
    </form>
    </div>
  </div>
</body>
</html>
