#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"

#include "Renderer.h"

#include <numeric>
using namespace Walnut;

class MainLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
			
			ImGui::Text("Current Resolution: %i x %i", m_ViewportWidth, m_ViewportHeight);

			if (ImGui::Button("Generate")) {
				Render();
			 }

			ImGui::SameLine(); ImGui::Checkbox("Evolution", &m_EvolutionActive);

			ImGui::Separator();

			ImGui::Text("Player Count: %i", m_ViewportHeight * m_ViewportWidth);
			ImGui::SliderInt("Player Size", &m_Renderer.PlayerSizeInPixels, 1, 128);
			ImGui::SliderInt("Evolution Speed", &m_Renderer.EvolutionSpeed, 1, 512);
			ImGui::SliderFloat("Probability W", &ProbabilityW, 0.f, 1.f);

			const auto payoff = m_Renderer.GetPayoff();

			if (!payoff->empty()) {
				auto [bestPlayer, payoff] = m_Renderer.GetBestPlayer();
				ImGui::Text("Best Player:");
				ImGui::BulletText("p: %f", bestPlayer.p);
				ImGui::BulletText("q: %f", bestPlayer.q);
				ImGui::BulletText("Payoff: %f", payoff);
			}

			ImGui::Separator();
			
			ImGui::Text("Visualization:");
			ImGui::RadioButton("Payoff", &m_Renderer.VisualizationType, 0); ImGui::SameLine();
			ImGui::RadioButton("Strategy", &m_Renderer.VisualizationType, 1);
			
			if (m_Renderer.VisualizationType == 1) {
				ImGui::ColorEdit4("Strategy 0", (float*)&m_Renderer.ColorsStrategy[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::SameLine(); ImGui::Text("Rational (0, 0)");
				ImGui::ColorEdit4("Strategy 1", (float*)&m_Renderer.ColorsStrategy[1], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::SameLine(); ImGui::Text("Other");
				ImGui::ColorEdit4("Strategy 2", (float*)&m_Renderer.ColorsStrategy[2], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::SameLine(); ImGui::Text("Fair (1/2, 1/2)");
			}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

		ImGui::Begin("Viewport");
			m_ViewportWidth = ImGui::GetContentRegionAvail().x / m_Renderer.PlayerSizeInPixels;
			m_ViewportHeight = ImGui::GetContentRegionAvail().y / m_Renderer.PlayerSizeInPixels;

			auto image = m_Renderer.GetImage();
			if(image)
				ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth() * m_Renderer.PlayerSizeInPixels, (float)image->GetHeight() * m_Renderer.PlayerSizeInPixels });

		ImGui::End();

		ImGui::Begin("Payoff");
			if (!payoff->empty()) 
				ImGui::PlotHistogram("", payoff->data(), payoff->size(), 0, "Payoff", 0.0f, 6.0f, ImGui::GetContentRegionAvail());
			
		ImGui::End();

		ImGui::PopStyleVar();
	}

	virtual void OnUpdate(float ts) {
		if (!(m_Renderer.GetImage() && m_EvolutionActive))
			return;

		m_Renderer.MutatePlayers();
		const auto payoff = m_Renderer.GetPayoff();
	}
private:

	void Render() {
		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
	}

	Renderer m_Renderer;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	bool m_EvolutionActive = false;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ultimatum Game";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<MainLayer>();
	/*app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});*/
	return app;
}