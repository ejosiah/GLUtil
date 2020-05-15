#include <glm/glm.hpp>
#include "../GLUtil/include/ncl/gl/Scene.h"
#include "../GLUtil/include/ncl/gl/pbr.h"
#include <iterator>

using namespace std;
using namespace ncl;
using namespace gl;
using namespace glm;

/* shader source code: for program that shows the cubemap */
static string vertexshader_show_source = {
    "#version 450 core\n"
    "layout (location = 0) in vec3 in_position;"
    "layout (location = 0) uniform mat4 MVP = mat4(1);"
    "out VS_FS { smooth vec3 cubemap_texcoord; } vs_out;"
    "void main() {"
    "gl_Position = MVP * vec4(in_position, 1);"
    "vs_out.cubemap_texcoord = in_position;"
    "}"
};

static string fragmentshader_show_source = {
    "#version 450 core\n"
    "in VS_FS { smooth vec3 cubemap_texcoord; } fs_in;"
    "layout (binding = 2) uniform samplerCube tex1;"
    "layout (location = 0) out vec4 out_color;"
    "void main() {"
    "out_color = texture(tex1, fs_in.cubemap_texcoord);"
    "}"
};


/* shader source code: for program that renders to the cubemap */
static string vertexshader_render_source = {
    "#version 450 core\n"
    "layout (location = 0) in vec3 in_position;"
    "out VS_GS { vec3 position; } vs_out;"
    "void main() {"
    "vs_out.position = in_position;"
    "}"
};

static string geometryshader_render_source = {
    "#version 450 core\n"
    "layout (triangles) in;"
    "layout (triangle_strip, max_vertices = 3 * 6) out;"
    "in VS_GS { vec3 position; } gs_in[];"
    "out GS_FS { vec4 color; } gs_out;"
    /* example: give each face a different color */
    "const vec4 mycolorarray[6] = {"
    "vec4(1, 0, 0, 1), vec4(0, 1, 1, 1),"
    "vec4(0, 1, 0, 1), vec4(1, 0, 1, 1),"
    "vec4(0, 0, 1, 1), vec4(1, 1, 0, 1) };"
    "void main() {"
    "for (int face = 0; face < 6; face++) {"
    /* set layer that gets this primitive: */
    "   gl_Layer = face;"
    /* pass the triangle trough to each layer with different colors */
    "   for (int vertex = 0; vertex < 3; vertex++) {"
    "   gl_Position = vec4(gs_in[vertex].position, 1);"
    "   gs_out.color = mycolorarray[face];"
    "   EmitVertex();"
    "} EndPrimitive(); } }"
};

static string fragmentshader_render_source = {
    "#version 450 core\n"
    "in GS_FS { vec4 color; } fs_in;"
    "layout (location = 0) out vec4 out_color;"
    "void main() {"
    "out_color = fs_in.color;"
    "}"
};



class LayeringScene : public Scene {
public:
	LayeringScene() :Scene("Laying") {
        addShader("render", GL_VERTEX_SHADER, vertexshader_render_source);
        addShader("render", GL_GEOMETRY_SHADER, geometryshader_render_source);
        addShader("render", GL_FRAGMENT_SHADER, fragmentshader_render_source);

        addShader("show", GL_VERTEX_SHADER, vertexshader_show_source);
        addShader("show", GL_FRAGMENT_SHADER, fragmentshader_show_source);
	}

	void init() override {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glGenFramebuffers(1, &framebuffer);
        glGenTextures(1, &texture);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        cube = Cube{ 2 };
        Mesh mesh;
        mesh.positions.emplace_back(0, 0, 0);
        mesh.positions.emplace_back(1, 0, 0);
        mesh.positions.emplace_back(0, 1, 0);
        triangle = new ProvidedMesh(mesh);

        /* setup texture (cubemap) */
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA16, texture_size.x, texture_size.y);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        /* bind texture to unit 2 (the same unit used in the fragmentshader to show the cubemap) */
        glBindTextureUnit(2, texture);


        /* setup framebuffer with cubemap attached */
        
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /* bind texture to unit 2 (the same unit used in the fragmentshader to show the cubemap) */
        glBindTextureUnit(2, texture);

        /* setup framebuffer with cubemap attached */
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        CheckFramebuffer(framebuffer, GL_FRAMEBUFFER);
	}

    void CheckFramebuffer(GLuint framebuffer, GLenum target)
    {
        GLenum status = glCheckNamedFramebufferStatus(framebuffer, target);
        stringstream ss;
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            ss << "OpenGL Framebuffer Error:  \t";
            if (status == GL_FRAMEBUFFER_UNDEFINED)
                ss << "undefined framebuffer";
            if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
                ss << "a necessary attachment is uninitialized";
            if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
                ss << "no attachments";
            if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
                ss << "incomplete draw buffer";
            if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
                ss << "incomplete read buffer";
            if (status == GL_FRAMEBUFFER_UNSUPPORTED)
                ss << "combination of attachments is not supported";
            if (status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
                ss << "number if samples for all attachments does not match";
             throw ss.str();
        }
    }

    void display() override {
        static float angle = 0;
        angle += 0.016f;

        mat4 MVP =
            perspective(radians(45.0f), 1.33f, 0.1f, 100.0f) *
            lookAt(vec3(5 * cos(angle), 2 * sin(angle * 0.5f), 5 * sin(angle)), vec3(0), vec3(0, 1, 0));

        /* first draw 1 triangle into the cubemap layers */
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, texture_size.x, texture_size.y);

        glClearColor(0.5, 0.5, 0.5, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader("render")([&] {
            shade(triangle);
         });


        /* then show the cubemap */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, _width, _height);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader("show")([&] {
            send("MVP", MVP);
            shade(&cube);
         });

    }

private:
    Cube cube;
    GLuint texture;
    GLuint framebuffer;
    ProvidedMesh* triangle;
    uvec2 texture_size{ 1024, 1024 };
};