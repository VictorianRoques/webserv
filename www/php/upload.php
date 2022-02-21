<?php
$uploaddir = '/Users/viroques/Documents/webserv/www/upload/';
$uploadfile = $uploaddir . basename($_FILES['userfile']['name']);

$ip = getenv('UPLOAD_DEST') . "\n";
echo $ip;
echo '<pre>';
if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile)) {
    echo "File is valid, and was successfully uploaded.\n";
} else {
    echo "Possible file upload attack!\n";
}
print "</pre>";

?>