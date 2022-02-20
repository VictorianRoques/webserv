import os
import cgi, cgitb

cgitb.enable()

uploadDest = os.environ['UPLOAD_DEST']

form = cgi.FieldStorage()
fileitem = form['filename']

 
if fileitem.filename:
	fn = os.path.basename(fileitem.filename)
	open(uploadDest + fn, 'wb').write(fileitem.file.read())
	message = 'The file "' + fn + '" was uploaded successfully'
else:
	message = 'No file was uploaded'
print("Content-Type: text/html")
print("\
<html>\n\
	<body>\n\
		<p> " + message + " </p>\n\
	</body>\n\
</html>\n\
")