<!DOCTYPE html>

<html>
<head>
  <title>Registrace</title>
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
  <?php
  include 'ituscript.php';
  itu_dbOpen();
  $ishelegit = itu_getPass($_POST["username"]);

  if (strlen($_POST["password"]) < 5)
  {
  	echo '<div class="container">
            <p>Zadajte heslo ktore ma aspon 5 znakov</p>
          </div>';
  	echo '<div class="top-panel">
            <ul>
              <li><a href="registration.php" >Navrat na stranku</a></li>
            </ul>
          </div>';


  }
  else if (strlen($_POST["password"]) > 4)
  {
  	if ($ishelegit == '')
  		{
        echo '<div class="container">
                <p>Ucet vytvoren</p>
              </div>';
  		itu_addUser($_POST["username"], $_POST["password"]);
      echo '<div class="top-panel">
              <ul>
                <li><a href="index.php" >Navrat na stranku</a></li>
              </ul>
            </div>';
  		}

  	else if ($ishelegit != '')
  		{
        echo '<div class="container">
                <p>Zadajte ine meno</p>
              </div>';
      echo '<div class="top-panel">
              <ul>
                <li><a href="registration.php" >Navrat na stranku</a></li>
              </ul>
            </div>';
  		}
  }

  itu_dbClose();
  ?>

</body>
</html>
