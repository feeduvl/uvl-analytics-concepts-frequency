import subprocess
from flask import Flask, json, jsonify, logging, request
from logging.config import dictConfig

with open('./config.json') as config_file:
    CONFIG = json.load(config_file)

app = Flask(__name__)

dictConfig({
    'version': 1, 'root': {'level':'INFO'}})


@app.route("/hitec/classify/concepts/frequency/", methods=["POST"])
def post_classification_result():
    app.logger.debug('/hitec/classify/concepts/frequency/ called')

    # app.logger.debug(request.data.decode('utf-8'))
    content = json.loads(request.data.decode('utf-8'))

    app.logger.info("Running topic detection on: "+content["classify"])

    listfiles = subprocess.Popen(["ls"],
                                stdout=subprocess.PIPE,
                                universal_newlines=True)

    (stdout_data, stderr_data) = listfiles.communicate();

    app.logger.info("All files: "+stdout_data)

    process = subprocess.Popen(['./feed_uvl_finding_comparatively', "run_algorithm", "2", "algo1", "res/frequencies.txt", content["classify"], "20"],
                               stdout=subprocess.PIPE,
                               universal_newlines=True)

    (stdout_data, stderr_data) = process.communicate()

    r = json.loads(stdout_data)

    app.logger.info("Returning result: "+str(r))

    return jsonify(r), 200


if __name__ == "__main__":
    app.run(debug=False, threaded=False, host=CONFIG['HOST'], port=CONFIG['PORT'])