import pandas as pd
import numpy as np
import subprocess
import json
import os


ground_truths_filepath = "/opt/containers/validation/ground truth all.xlsx"


def get_true_and_false_pos_neg(target_concepts, actual_concepts, single_words):
    '''

    :param target_concepts: list of lists of strings
    :param actual_concepts:
    :param single_words: if true, actual_concepts is a list of strings, else a list of lists of strings
    :return: (true_positives, false_positives, true_negatives, false_negatives)
    '''

    false_positives = [True for a in actual_concepts]
    false_negatives = [True for t in target_concepts]
    # no negative votes in actual_concepts

    for ind_t, t in enumerate(target_concepts):
        for ind_a, a in enumerate(actual_concepts):
            for w in t:
                # print("Comparing actual: ", a, " with expected: ", w)
                if single_words:
                    if w == a:
                        false_positives[ind_a] = False
                        false_negatives[ind_t] = False
                        print("Found lemma: "+a)
                        break
                else:
                    for actual_lemma in a:
                        if w == actual_lemma:
                            false_positives[ind_a] = False
                            false_negatives[ind_t] = False
                            print("Found lemma: " + actual_lemma)
                            break

    fp = np.count_nonzero(false_positives)
    fn = np.count_nonzero(false_negatives)

    tp = len(false_positives) - fp

    return tp, fp, 0, fn


def precision(tp, fp):
    return tp/(tp+fp)


def recall(tp, fn):
    return tp/(tp + fn)


def F1_score(tp, fp, fn):
    return tp/(tp + (0.5*(fp + fn)))


def validate_rbai(docs, logger):
    """
    Compute precision, recall and F1 metric for the dataset
    :param text:
    :param num_concepts:
    :return:
    """
    texts = "".join([doc["text"] + "/n" for doc in docs])
    df = pd.read_excel(ground_truths_filepath)
    tokenize_args = ["./tokenize"]
    target_concepts = (concept for ind, concept in df["Segment"].items())

    tokenize_args += target_concepts

    tokenize_output_ = subprocess.run(tokenize_args, capture_output=True)
    j = json.loads(tokenize_output_.stdout.decode("utf-8", errors="replace") ) # should never be an error
    lemmatized_target_concepts = j["concepts"]

    try_num_concepts = [10, 20, 30, 40]

    for num_concepts in try_num_concepts:

        rbai_args = ['./lib/feed_uvl_rbai',
                "run_algorithm",
                "2",
                "/app/lib/res/frequencies.txt",
                "/app/lib/res/stopwords.txt",
                "/app/lib/res/lemmatization-en.txt",
                texts,
                str(num_concepts),
                "test"]

        output = subprocess.run(rbai_args, capture_output=True)

        o = output.stdout.decode("utf-8", errors="replace")
        errors = output.stderr.decode("utf-8", errors="ignore")

        #print("fcic output: "+o)

        if errors is not None and errors != "":
            logger.error("Program errors: " + errors)

        result = json.loads(o)

        (tp, fp, tn, fn) = get_true_and_false_pos_neg(lemmatized_target_concepts, result["topics"]["concepts"], False)

        logger.info("Precision for N=", num_concepts, ": "+str(precision(tp, fp)))

        logger.info("Recall for N=", num_concepts, ": "+str(recall(tp, fn)))

        logger.info("F1 for N=", num_concepts, ": "+str(F1_score(tp, fp, fn)))


def validate_fcic(docs, logger):
    """
    Compute precision, recall and F1 metric for the dataset
    :param text:
    :param num_concepts:
    :return:
    """
    texts = "".join([doc["text"] + "/n" for doc in docs])
    df = pd.read_excel(ground_truths_filepath)
    tokenize_args = ["./tokenize"]
    target_concepts = (concept for ind, concept in df["Segment"].items())

    tokenize_args += target_concepts

    #print(args)

    tokenize_output_ = subprocess.run(tokenize_args, capture_output=True)
    j = json.loads(tokenize_output_.stdout.decode("utf-8", errors="replace") ) # should never be an error
    lemmatized_target_concepts = j["concepts"]

    try_num_concepts = [10, 20, 30, 40]

    for num_concepts in try_num_concepts:

        fcic_args = ['./lib/feed_uvl_fcic',
                "run_algorithm",
                "1",
                "/app/lib/res/frequencies.txt",
                "/app/lib/res/stopwords.txt",
                "/app/lib/res/lemmatization-en.txt",
                texts,
                str(num_concepts),
                "test",
                "/opt/containers/frequency-data/"+"Small"]

        output = subprocess.run(fcic_args, capture_output=True)

        o = output.stdout.decode("utf-8", errors="replace")
        errors = output.stderr.decode("utf-8", errors="ignore")

        if errors is not None and errors != "":
            logger.error("Program errors: " + errors)

        result = json.loads(o)

        (tp, fp, tn, fn) = get_true_and_false_pos_neg(lemmatized_target_concepts, result["topics"]["concepts"], True)

        logger.info("Precision for N=", num_concepts, ": "+str(precision(tp, fp)))

        logger.info("Recall for N=", num_concepts, ": "+str(recall(tp, fn)))

        logger.info("F1 for N=", num_concepts, ": "+str(F1_score(tp, fp, fn)))
