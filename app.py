import subprocess
from flask import Flask, json, jsonify, logging, request
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

    process = subprocess.Popen(['./lib/feed_uvl_finding_comparatively', content["params"]["command"], content["params"]["term_length"], "rbai", "res/frequencies.txt", texts, content["params"]["max_num_concepts"]],
                               stdout=subprocess.PIPE,
                               universal_newlines=True,
                               encoding="utf-8")

    (stdout_data, stderr_data) = process.communicate()

    app.logger.debug("Program output: "+stdout_data)

    return stdout_data, 200


@app.route("/hitec/classify/concepts/frequency-rbai/status", methods=["GET"])
def get_status():
    status = {
        "status": "operational",
    }

    return jsonify(status)


if __name__ == "__main__":
    app.run(debug=False, threaded=False, host=CONFIG['HOST'], port=CONFIG['PORT'])