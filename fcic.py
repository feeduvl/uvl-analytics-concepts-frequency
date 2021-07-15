import subprocess
from flask import Flask, json, jsonify, request
from logging.config import dictConfig

from find_occurences import find_occurences
from validate_test_input import validate_fcic

with open('config_fcic.json') as config_file:
    CONFIG = json.load(config_file)

app = Flask(__name__)

dictConfig({
    'version': 1, 'root': {'level':'DEBUG'}})

app.logger.info("Server starting now.")


@app.route("/hitec/classify/concepts/frequency-fcic/run", methods=["POST"])
def post_classification_result():
    app.logger.debug('/hitec/classify/concepts/frequency-fcic/run called')
    content = json.loads(request.data.decode('utf-8'))

    texts = "".join([doc["text"] + "\n" for doc in content["dataset"]["documents"]])

    try:
        dataset = content["params"]["corpus_dataset_name"]
    except KeyError as e:
        app.logger.error("Dataset was not defined, using Small")
        dataset = "Small"

    if dataset not in ("Small", "Medium"):
        app.logger.error("Got unexpected dataset name: '"+str(dataset)+"', using Small")
        dataset = "Small"

    args = ['./lib/feed_uvl_fcic',
            content["params"]["command"],
            content["params"]["term_length"],
            "/app/lib/res/frequencies.txt",
            "/app/lib/res/stopwords.txt",
            "/app/lib/res/lemmatization-en.txt",
            texts,
            content["params"]["max_num_concepts"],
            "deprecated param, refactor eventually",
            "/opt/containers/frequency-data/"+dataset]

    output = subprocess.run(args, capture_output=True)

    o = output.stdout.decode("utf-8", errors="replace")
    errors = output.stderr.decode("utf-8", errors="ignore")
    app.logger.debug("Program output: " + o)
    if errors is not None and errors != "":
        app.logger.error("Program errors: " + errors)

    result = json.loads(o)
    #perform validation
    validate_fcic(content["dataset"]["documents"], app.logger)

    return json.dumps(find_occurences(content, result, "feed_uvl_fcic", app.logger)), 200


@app.route("/hitec/classify/concepts/frequency-fcic/status", methods=["GET"])
def get_status():
    status = {
        "status": "operational",
    }

    return jsonify(status)


if __name__ == "__main__":
    app.run(debug=False, threaded=False, host=CONFIG['HOST'], port=CONFIG['PORT'])