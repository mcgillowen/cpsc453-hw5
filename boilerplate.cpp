// ==========================================================================
// An Object-Oriented Boilerplate Code for GLFW
//
// Author:  Kamyar Allahverdi, University of Calgary
// Date:    January 2017
//
// Modified by: Usman Alim
// April 2017
// ==========================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cstdio>
#include <cstring>


#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::strcmp;
using std::fscanf;

class Program {
  GLuint vertex_shader;
  GLuint tess_control_shader;
  GLuint tess_evaluation_shader;
  GLuint fragment_shader;

public:
  GLuint id;
  Program() {
    vertex_shader=0;
    tess_control_shader=0;
    tess_evaluation_shader=0;
    fragment_shader=0;
    id=0;
  }
  Program(string vertex_path, string tess_control_path, string tess_evaluation_path, string fragment_path) {
    init(vertex_path, tess_control_path, tess_evaluation_path, fragment_path);
  }
  void init(string vertex_path, string tess_control_path, string tess_evaluation_path, string fragment_path) {
    id=glCreateProgram();
    vertex_shader=addShader(vertex_path,GL_VERTEX_SHADER);
    tess_control_shader=addShader(tess_control_path,GL_TESS_CONTROL_SHADER);
    tess_evaluation_shader=addShader(tess_evaluation_path,GL_TESS_EVALUATION_SHADER);
    fragment_shader=addShader(fragment_path,GL_FRAGMENT_SHADER);
    if(vertex_shader) glAttachShader(id,vertex_shader);
    if(tess_control_shader) glAttachShader(id,tess_control_shader);
    if(tess_evaluation_shader) glAttachShader(id,tess_evaluation_shader);
    if(fragment_shader) glAttachShader(id,fragment_shader);

    glLinkProgram(id);
  }
  GLuint addShader(string path, GLuint type) {
    std::ifstream in(path);
    string buffer = [&in]{
      std::ostringstream ss{};
      ss << in.rdbuf();
      return ss.str();
    }();
    const char *buffer_array[]={buffer.c_str()};

    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, buffer_array, 0);
  	glCompileShader(shader);

    // Compile results
    GLint status;
  	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  	if (status == GL_FALSE)
  	{
  		GLint length;
  		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
  		string info(length, ' ');
  		glGetShaderInfoLog(shader, info.length(), &length, &info[0]);
  		cerr << "ERROR compiling shader:" << endl << endl;
  		cerr << info << endl;
  	}
    return shader;
  }
  ~Program() {
    glUseProgram(0);
    glDeleteProgram(id);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
  }
};

class VertexArray {
  std::map<string,GLuint> buffers;
  std::map<string,int> indices;
public:
  GLuint id;
  unsigned int count;
  VertexArray(int c) {
    glGenVertexArrays(1, &id);
    count = c;
  }

  VertexArray(const VertexArray &v) {
    glGenVertexArrays(1, &id);

    // Copy data from the old object
    this->indices = std::map<string, int>(v.indices);
    count = v.count;

    vector<GLuint> temp_buffers(v.buffers.size());

    // Allocate some temporary buffer object handles
    glGenBuffers(v.buffers.size(), &temp_buffers[0]);

    // Copy each old VBO into a new VBO
    int i = 0;
    for(auto &ent: v.buffers) {
      int size = 0;
      glBindBuffer(GL_ARRAY_BUFFER, ent.second);
      glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

      glBindBuffer(GL_COPY_READ_BUFFER, temp_buffers[i]);
      glBufferData(GL_COPY_READ_BUFFER, size, NULL, GL_STATIC_COPY);

      glCopyBufferSubData(GL_ARRAY_BUFFER,GL_COPY_READ_BUFFER, 0, 0, size);
      i++;
    }

    // Copy those temporary buffer objects into our VBOs

    i = 0;
    for(auto &ent: v.buffers) {
      GLuint buffer_id;
      int size = 0;
      int index = indices[ent.first];

      glGenBuffers(1, &buffer_id);

      glBindVertexArray(this->id);
      glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
      glBindBuffer(GL_COPY_READ_BUFFER, temp_buffers[i]);
      glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);

      // Allocate VBO memory and copy
      glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
      glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, size);
      string indexs = ent.first;

      buffers[ent.first] = buffer_id;
      indices[ent.first] = index;

      // Setup the attributes
      size = size/(sizeof(float)*this->count);
      glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);
      glEnableVertexAttribArray(index);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      i++;
    }

    // Delete temporary buffers
    glDeleteBuffers(v.buffers.size(), &temp_buffers[0]);
  }

  void addBuffer(string name, int index, vector<float> buffer) {
    GLuint buffer_id;
    glBindVertexArray(id);

    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, buffer.size()*sizeof(float), buffer.data(), GL_STATIC_DRAW);
    buffers[name]=buffer_id;
    indices[name]=index;

    int components=buffer.size()/count;
    glVertexAttribPointer(index, components, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);

    // unset states
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void updateBuffer(string name, vector<float> buffer) {
    glBindBuffer(GL_ARRAY_BUFFER, buffers[name]);
    glBufferData(GL_ARRAY_BUFFER, buffer.size()*sizeof(float), buffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  ~VertexArray() {
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    for(auto &ent: buffers)
  	  glDeleteBuffers(1, &ent.second);
  }
};

class Loader {
    string text;

public:
    Loader(string textToDisplay){
        text = textToDisplay;
    }

    VertexArray load() {

        string path = "cmuntt/gly_";

        vector<float> points;

        int pos = 0;
        int length = text.length();
        for (; pos < length; pos++) {
            int letter = text[pos];

            if (letter == 32) continue;

            float startPos[2] = {};

            float previousEndPoint[2] = {};

            string letterPath = path + std::to_string(letter);

            FILE * file = fopen(letterPath.c_str(),"r");

            bool openFile = true;
            if (file == NULL) {
                printf("Impossible to open the file\n");
                printf(letterPath.c_str());
                openFile = false;
            }

            while (openFile) {
                char lineType[2];
                int res = fscanf(file, "%s", lineType);

                if (res == EOF) {
                    break;
                }
                if (strcmp(lineType, "M") == 0) {
                    float coords[2];
                    fscanf(file, "%f %f\n", &coords[0], &coords[1]);
                    startPos[0] = coords[0];
                    startPos[1] = coords[1];
                    previousEndPoint[0] = coords[0];
                    previousEndPoint[1] = coords[1];
                } else if (strcmp(lineType, "C") == 0) {
                    float point1[2];
                    float point2[2];
                    float point3[2];
                    fscanf(file, "%f %f %f %f %f %f",
                        &point1[0], &point1[1],
                        &point2[0], &point2[1],
                        &point3[0], &point3[1]);


                    points.push_back(previousEndPoint[0]);
                    points.push_back(previousEndPoint[1]);
                    points.push_back(point1[0]);
                    points.push_back(point1[1]);
                    points.push_back(point2[0]);
                    points.push_back(point2[1]);
                    points.push_back(point3[0]);
                    points.push_back(point3[1]);

                    previousEndPoint[0] = point3[0];
                    previousEndPoint[1] = point3[1];
                } else if (strcmp(lineType, "L") == 0) {
                    float point0[2] = {};
                    point0[0] = previousEndPoint[0];
                    point0[1] = previousEndPoint[1];
                    float point1[2];

                    fscanf(file, "%f %f", &point1[0], &point1[1]);

                    float middle1[2] = {};
                    float middle2[2] = {};

                    middle1[0] = point0[0] * 0.75 + point1[0] * 0.25;
                    middle1[1] = point0[1] * 0.75 + point1[1] * 0.25;

                    middle2[0] = point0[0] * 0.25 + point1[0] * 0.75;
                    middle2[1] = point0[1] * 0.25 + point1[1] * 0.75;

                    points.push_back(point0[0]);
                    points.push_back(point0[1]);
                    points.push_back(middle1[0]);
                    points.push_back(middle1[1]);
                    points.push_back(middle2[0]);
                    points.push_back(middle2[1]);
                    points.push_back(point1[0]);
                    points.push_back(point1[1]);
                } else if (strcmp(lineType, "Z") == 0) {
                    float point0[2] = {};
                    point0[0] = previousEndPoint[0];
                    point0[1] = previousEndPoint[1];
                    float point1[2] = {};
                    point1[0] = startPos[0];
                    point1[1] = startPos[1];

                    float middle1[2] = {};
                    float middle2[2] = {};

                    middle1[0] = point0[0] * 0.75 + point1[0] * 0.25;
                    middle1[1] = point0[1] * 0.75 + point1[1] * 0.25;

                    middle2[0] = point0[0] * 0.25 + point1[0] * 0.75;
                    middle2[1] = point0[1] * 0.25 + point1[1] * 0.75;

                    points.push_back(point0[0]);
                    points.push_back(point0[1]);
                    points.push_back(middle1[0]);
                    points.push_back(middle1[1]);
                    points.push_back(middle2[0]);
                    points.push_back(middle2[1]);
                    points.push_back(point1[0]);
                    points.push_back(point1[1]);
                }
            }

        }

        VertexArray va(points.size());

        va.addBuffer("v", 0, points);

        return va;

    }
};

void render(Program &program, VertexArray &va)
{
	// clear screen to a dark grey colour
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

  // Line smoothing hints.
  glEnable (GL_LINE_SMOOTH);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);


	glUseProgram(program.id);
	glBindVertexArray(va.id);

  glPatchParameteri( GL_PATCH_VERTICES, 4 );
	glDrawArrays( GL_PATCHES, 0, va.count );

	glBindVertexArray(0);
	glUseProgram(0);

}

int main(int argc, char *argv[])
{
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initialize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback([](int error, const char* description){
    cout << "GLFW ERROR " << error << ":" << endl;
    cout << description << endl;
  });

  // Use OpenGL 4.1
	GLFWwindow *window = 0;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(512, 512, "CPSC 453 OpenGL Tessellation Boilerplate", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

  Program p("vertex.glsl", "tessControl.glsl", "tessEvaluation.glsl", "fragment.glsl");
  //VertexArray va(4);
  // va.addBuffer("v", 0, vector<float>{
  //   -1.0,-1.0,
  //   -1.0,1.0,
  //     1.0,1.0,
  //     1.0,-1.0
  // });

    Loader l("Q");
    VertexArray va = l.load();



	// run an event-triggered main loop
	while (!glfwWindowShouldClose(window))
	{
    // render
		render(p,va);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	cout << "The End" << endl;
	return 0;
}
