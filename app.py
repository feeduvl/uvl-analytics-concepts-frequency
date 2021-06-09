import subprocess
from flask import Flask, json, jsonify, request
from logging.config import dictConfig

with open('./config.json') as config_file:
    CONFIG = json.load(config_file)

app = Flask(__name__)

dictConfig({
    'version': 1, 'root': {'level':'DEBUG'}})

app.logger.info("Server starting now.")


@app.route("/hitec/classify/concepts/frequency-rbai/run", methods=["POST"])
def post_classification_result():
    app.logger.debug('/hitec/classify/concepts/frequency-rbai/run called')

    # app.logger.debug(request.data.decode('utf-8'))
    content = json.loads(request.data.decode('utf-8'))

    texts = [doc["text"] + "\n" for doc in content["dataset"]["documents"]]
    texts = "".join(texts)
    texts = texts

    app.logger.debug("Processing: "+texts)
    args = ['./lib/feed_uvl_finding_comparatively',
            content["params"]["command"],
            content["params"]["term_length"],
            "rbai",
            "/app/lib/res/frequencies.txt",
            "/app/lib/res/stopwords.txt",
            "/app/lib/res/lemmatization-en.txt",
            texts,
            content["params"]["max_num_concepts"]]
    output = subprocess.run(args,
                            capture_output=True)

    o = output.stdout.decode("utf-8", errors="ignore")
    errors = output.stderr.decode("utf-8", errors="ignore")
    app.logger.debug("Program output: " + o)
    if errors is not None and errors != "":
        app.logger.error("Program errors: " + errors)

    return o, 200


@app.route("/hitec/classify/concepts/frequency-rbai/status", methods=["GET"])
def get_status():
    status = {
        "status": "operational",
    }

    return jsonify(status)


if __name__ == "__main__":
    app.run(debug=False, threaded=False, host=CONFIG['HOST'], port=CONFIG['PORT'])