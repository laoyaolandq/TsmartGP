from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
import os
import urllib
import urlparse
import cgi
import sys
import webbrowser

reload(sys)

sys.setdefaultencoding('utf8')

root_path = os.path.abspath(sys.argv[1])
class TestHTTPHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        global root_path
        r = []
        parseResult = urlparse.urlparse(self.path)
        param_dict = urlparse.parse_qs(parseResult.query)
        f = parseResult.path
        if not f == '/':
            fp  = open(os.curdir + f)
            self.protocal_version = "HTTP/1.1"
            self.send_response(200)  
            self.send_header("Welcome", "Contact")    
            self.send_header("Access-Control-Allow-Origin", "*")            
            self.end_headers()
            self.wfile.write(fp.read())
            fp.close()
            return
        if 'dir' in param_dict:
            r=['<ul class="jqueryFileTree" style="display: none;">']
            try:
                r = ['<ul class="jqueryFileTree" style="display: none;">']
                d = ''.join(param_dict['dir'])
                d = urlparse.unquote(d)
                d = os.path.join(root_path,d)
                d = d.strip();
                for f in os.listdir(d):
                    ff=os.path.join(d,f)
                    if os.path.isdir(ff):
                        r.append('<li class="directory collapsed"><a rel="%s/">%s</a></li>' % (ff,f))
                    else:
                        e=os.path.splitext(f)[1][1:] # get .ext and remove dot
                        r.append('<li class="file ext_%s"><a rel="%s">%s</a></li>' % (e,ff,f))
                r.append('</ul>')
            except Exception as e:
                r.append('Could not load directory: %s' % str(e))
            r.append('</ul>')
        if 'file' in param_dict:
            r=[]
            try:
                r=[]
                d = ''.join(param_dict['file'])
                d = urlparse.unquote(d)
                d = os.path.join(root_path,d)
                fileObj = open(d)
                allText = fileObj.read()
                fileObj.close()
                cgi.escape(allText)
                r.append(allText)
            except Exception as e:
                r.append('Could not load file: %s' % str(e))
        try:
            self.protocal_version = "HTTP/1.1"
            self.send_response(200)  
            self.send_header("Welcome", "Contact")    
            self.send_header("Access-Control-Allow-Origin", "*")            
            self.end_headers()
            self.wfile.write(''.join(r).encode())
        except Exception as e:
            print("error send: %s" % str(e))

port = 8080

httpd = HTTPServer(('', port),  TestHTTPHandler)
print("Serve on port: " + str(httpd.server_port))
webbrowser.open("http://localhost:8080/index.html")
httpd.serve_forever()
