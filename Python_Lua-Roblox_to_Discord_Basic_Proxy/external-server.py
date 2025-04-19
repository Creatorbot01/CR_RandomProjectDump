from flask import Flask, request
import requests
 
app = Flask(__name__)
 
@app.route('/')
def nothinges():
    return 'yeah there nothing im just putting this incase some guy wanna just put the main link in'
 
#the actual proxy thingi magic
@app.route('/sendmessage', methods=['POST'])
def handle_post():
    if request.method == 'POST':
        request_data = request.get_json()
		#sanity check to stop the entire api from dying
        if request_data:
            if ('message' in request_data) == False:
                return "ERROR:message cannot be empty"
            if ('user' in request_data) == False:
                return "ERROR:user cannot be empty"
            if ('webhook' in request_data) == False:
                return "ERROR:webhook cannot be empty"
            if ('source' in request_data) == False:
                return "ERROR:source cannot be empty"
 
            check = "discordapp.com/api/webhooks"
            if (check in request_data["webhook"]) == False:
                return "ERROR:only use discord webhook only"
 
			#just cleaning up to make it readable
            message = request_data['message']
            user = request_data['user']
            urlWebHook = request_data["webhook"]
            source = request_data['source']
 
            mUrl = urlWebHook
            data ={
                "content": "",
                "embeds" : [{
                    "title" : "From : "+user,
                    "description" : message,
                    "footer": {
                        "text": "Message send from "+source
                    }
                }]
            }
 
            response = requests.post(mUrl, json=data)
            return response.content
        else:
            return "ERROR:No data were send"
    else:
        return "ERROR:Incorrect method(need POST)"