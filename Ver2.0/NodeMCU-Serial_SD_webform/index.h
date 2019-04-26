const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <body>
        <h3> Health Kit Form </h3>
        <form action="/upload_data "method="POST">        
          Unique ID:<br>
          <input type="text" name="unique_id" value="">
          <br>
          <br><br>
          <input type="submit" value="Upload Data">
        </form>
        <br><br>
        <a href="/registration"><button> New Registration </button> </a>
    </body>
</html>
)=====";

const char Registration_PAGE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <body>
        <h3> Health Kit Registration Form </h3>
        <form action="/action_page"method="POST">        
          First name:<br>
          <input type="text" name="firstname" value="">
          <br>
          Last name:<br>
          <input type="text" name="lastname" value="">
          <br>
          Gender:
          <input type="radio" name="gender" value="male"> Male
          <input type="radio" name="gender" value="female"> Female
          <input type="radio" name="gender" value="Other"> Other
          <br>
          Age:
          <input type="number" name="age" min="1" max="100">
          <br><br>
          Unique ID:<br>
          <input type="text" name="unique_id" value="">
          <br>
          <br><br>
          <input type="submit" value="Submit">
        </form>

    </body>
</html>
)=====";
