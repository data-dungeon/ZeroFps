
#include "../zerofpsv2/basic/vector2.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"
#include "../zerofpsv2/render/render.h"

#include <vector>
#include <string>
using namespace std;

class AmbientSoundAreas
{
public:

	AmbientSoundAreas();
	~AmbientSoundAreas();

	void Draw(Render* pkRender);
	bool Save(string strFileName);
	bool Load(string strFileName, ZGuiListbox* pkList);

	void GetAllPointsInAmbientArea(string strAreaName, vector<Vector3>& kPoints);
	void ClearAllPointsInAmbientArea(string strArea);
	bool MoveAmbientAreaPoint(string strArea, Vector2 old_pos, Vector2 new_pos);
	bool AddPointToAmbientArea(string strArea, Vector2 point);
	void SetAmbientSound(string strArea, string strFile);
	void RemoveAllAmbientAreas();
	void RemoveAmbientArea(string strArea);
	void CreateNewAmbientArea(string strArea);
	string GetAmbientSound(string strArea);

	bool m_bShowAmbientSoundAreas;
	bool m_bAddPointsToSoundArea;
	string m_strAmbientAreaEdited;

private:

	struct AmbientAreaInfo
	{
		vector<Vector2*> m_kPolygon;
		string m_strFileName;
		string m_strAreaName;
	};

	vector<AmbientAreaInfo> m_kAmbientAreas;
};