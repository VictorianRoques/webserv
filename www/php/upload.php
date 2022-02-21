<?php
$uploaddir = getenv('UPLOAD_DEST');
$uploadfile = $uploaddir . basename($_FILES['userfile']['name']);

echo '<pre>';
if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile)) {
    echo "File is valid, and was successfully uploaded.\n";
    echo "Your file is available " . "<a href='" . $uploadfile . "'>here<a/>";
} else {
    echo "Possible file upload attack!\n";
}
print "</pre>";

?>