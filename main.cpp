#include "StudentDatabase.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <cctype>
#include <exception>
#include <limits>
#include <string>

namespace {
struct Form {
    int id = 1;
    char first[128] = {};
    char last[128] = {};
    char group[128] = {};
    int year = 2026;
    float gpa = 0.0f;
};

bool has_text(const char* value) {
    for (; *value; ++value) {
        if (!std::isspace(static_cast<unsigned char>(*value))) return true;
    }
    return false;
}

enum class View { All, ById, ByYears };

void draw_ui(StudentDatabase& database, Form& form, int& search_id,
             int& min_year, int& max_year, View& view, std::string& message) {
    ImGui::SetNextWindowSize(ImVec2(1000, 680), ImGuiCond_FirstUseEver);
    ImGui::Begin("Students");
    ImGui::Text("Students: %d", database.size());
    ImGui::Separator();

    ImGui::BeginChild("Controls", ImVec2(300, 0), ImGuiChildFlags_Borders);
    ImGui::TextUnformatted("Add student");
    ImGui::InputInt("ID##add", &form.id);
    ImGui::InputText("First name", form.first, sizeof(form.first));
    ImGui::InputText("Last name", form.last, sizeof(form.last));
    ImGui::InputText("Group", form.group, sizeof(form.group));
    ImGui::InputInt("Admission year", &form.year);
    ImGui::InputFloat("GPA", &form.gpa, 0, 0, "%.2f");
    if (ImGui::Button("Add")) {
        if (!has_text(form.first) || !has_text(form.last) || !has_text(form.group) ||
            form.year < 0 || form.gpa < 0.0f || form.gpa > 5.0f) {
            message = "Enter names, group, nonnegative year and GPA from 0 to 5.";
        } else if (!database.add(Student(form.id, form.first, form.last, form.group,
                                         form.year, form.gpa))) {
            message = "This ID already exists.";
        } else {
            message = "Student added.";
            form = Form{};
        }
    }

    ImGui::Separator();
    ImGui::TextUnformatted("Find or remove by ID");
    ImGui::InputInt("ID##search", &search_id);
    if (ImGui::Button("Find by ID")) {
        view = View::ById;
        message = database.contains(search_id) ? "Student found." : "Student not found.";
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove")) {
        message = database.remove(search_id) ? "Student removed." : "Student not found.";
    }

    ImGui::Separator();
    ImGui::TextUnformatted("Find by admission year");
    ImGui::InputInt("From", &min_year);
    ImGui::InputInt("To", &max_year);
    if (ImGui::Button("Find by years")) {
        if (min_year > max_year) message = "From must not exceed To.";
        else { view = View::ByYears; message.clear(); }
    }
    ImGui::SameLine();
    if (ImGui::Button("Show all")) { view = View::All; message.clear(); }
    if (!message.empty()) { ImGui::Separator(); ImGui::TextWrapped("%s", message.c_str()); }
    ImGui::EndChild();

    ImGui::SameLine
    ImGui::BeginChild("Results", ImVec2(0, 0), ImGuiChildFlags_Borders);
    const int from = view == View::ByYears ? min_year : std::numeric_limits<int>::min();
    const int to = view == View::ByYears ? max_year : std::numeric_limits<int>::max();
    auto rows = view == View::ById ? DynamicArray<Student>() :
                database.find_by_admission_year_range(from, to);
    if (view == View::ById && database.contains(search_id)) {
        rows.resize(1);
        rows[0] = database.get_by_id(search_id);
    }
    if (view == View::ById) {
        ImGui::Text("ID: %d", search_id);
    } else if (view == View::ByYears) {
        ImGui::Text("Admission years: %d - %d", min_year, max_year);
    } else {
        ImGui::TextUnformatted("All students");
    }
    if (ImGui::BeginTable("StudentsTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                         ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY)) {
        const char* names[] = {"ID", "First name", "Last name", "Group", "Year", "GPA"};
        for (const char* name : names) ImGui::TableSetupColumn(name);
        ImGui::TableHeadersRow();
        for (int i = 0; i < rows.get_size(); ++i) {
            const Student& student = rows[i];
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text("%d", student.get_id());
            ImGui::TableNextColumn(); ImGui::TextUnformatted(student.get_first_name().c_str());
            ImGui::TableNextColumn(); ImGui::TextUnformatted(student.get_last_name().c_str());
            ImGui::TableNextColumn(); ImGui::TextUnformatted(student.get_group().c_str());
            ImGui::TableNextColumn(); ImGui::Text("%d", student.get_admission_year());
            ImGui::TableNextColumn(); ImGui::Text("%.2f", student.get_gpa());
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
    ImGui::End();
}
} // namespace

int main() {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1200, 750, "Student database", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    StudentDatabase database;
    Form form;
    int search_id = 1, min_year = 2000, max_year = 2030;
    View view = View::All;
    std::string message;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        try { draw_ui(database, form, search_id, min_year, max_year, view, message); }
        catch (const std::exception& error) { message = error.what(); }
        ImGui::Render();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.10f, 0.11f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}
