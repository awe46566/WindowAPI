#pragma once

// N개의 Widget 관리한다.
// Panel들은 UIManager가 관리해도 되고 Scene이 관리해도 되고..

class UIPanel
{
public:
	~UIPanel();
	void AddWidget(class Widget* widget);

	void Show() { _visible = true; }
	void Hide() { _visible = false; }
	bool IsVisible() const { return _visible; }

	void Update(float deltaTime);
	void Render(HDC hdc);

private:
	vector<class Widget*> _widgets;
	bool _visible = true;
};

