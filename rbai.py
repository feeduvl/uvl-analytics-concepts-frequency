import subprocess
import json
from flask import Flask, jsonify, request
from flask import json as fjson
from logging.config import dictConfig

from find_occurences import find_occurences
from validate_test_input import validate_rbai

with open('config_rbai.json') as config_file:
    CONFIG = fjson.load(config_file)

app = Flask(__name__)

dictConfig({
    'version': 1, 'root': {'level':'DEBUG'}})

app.logger.info("Server starting now.")


@app.route("/hitec/classify/concepts/frequency-rbai/run", methods=["POST"])
def post_classification_result():
    app.logger.debug('/hitec/classify/concepts/frequency-rbai/run called')

    # app.logger.debug(request.data.decode('utf-8'))
    content = fjson.loads(request.data.decode('utf-8'))

    texts = [doc["text"] + "\n" for doc in content["dataset"]["documents"]]
    texts = "".join(texts)
    texts = texts

    args = ['./lib/feed_uvl_rbai',
            content["params"]["command"],
            content["params"]["term_length"],
            "/app/lib/res/frequencies.txt",
            "/app/lib/res/stopwords.txt",
            "/app/lib/res/lemmatization-en.txt",
            texts,
            content["params"]["max_num_concepts"],
            "deprecated param, refactor eventually"]

    output = subprocess.run(args, capture_output=True)

    o = output.stdout.decode("utf-8", errors="replace")
    errors = output.stderr.decode("utf-8", errors="ignore")
    app.logger.debug("Program output: " + o)
    if errors is not None and errors != "":
        app.logger.error("Program errors: " + errors)

    result = json.loads(o)

    #perform validation
    #validate_rbai(content["dataset"]["documents"], app.logger)

    return json.dumps(find_occurences(content, result, "feed_uvl_rbai", app.logger)), 200


@app.route("/hitec/classify/concepts/frequency-rbai/status", methods=["GET"])
def get_status():
    status = {
        "status": "operational",
    }

    return jsonify(status)


if __name__ == "__main__":
    app.run(debug=False, threaded=False, host=CONFIG['HOST'], port=CONFIG['PORT'])