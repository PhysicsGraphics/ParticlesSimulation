all: TransformFeedbackGL

TransformFeedbackGL:
	g++ -o out TransformFeedbackGLSL.cpp -lGL -lglut -lGLEW
