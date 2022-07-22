#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			GraphHandle HeartGraph;
			FontPool UI;

			int intParam[3]{ 0 };
			float floatParam[4]{ 0 };
		public:
			void Set(void) noexcept {
				HeartGraph = GraphHandle::Load("data/UI/Heart.png");
			}
			void Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto Green = GetColor(43, 163, 91);
				auto White = GetColor(212, 255, 239);
				//タイム,スコア
				{
					int xp1, yp1;
					xp1 = y_r(10);
					yp1 = y_r(10);
					UI.Get(y_r(20)).Get_handle().DrawString(xp1, yp1, "TIME", Green, White);
					UI.Get(y_r(20)).Get_handle().DrawStringFormat_RIGHT(xp1 + y_r(240), yp1, Green, White, "%d:%05.2f", intParam[0], floatParam[0]);

					yp1 += y_r(25);
					UI.Get(y_r(20)).Get_handle().DrawString(xp1, yp1, "SCORE", Green, White);
					UI.Get(y_r(20)).Get_handle().DrawStringFormat_RIGHT(xp1 + y_r(240), yp1, Green, White, "%d", intParam[1]);
				}
				//アイテム
				{
					float xp1, yp1;
					float xs1, ys1;

					xp1 = (float)y_r(50);
					yp1 = (float)DrawParts->disp_y - y_r(100);
					xs1 = (float)y_r(80) / 2;
					ys1 = (float)y_r(100) / 2;

					DrawBox((int)(xp1 - xs1), (int)(yp1 - ys1), (int)(xp1 + xs1), (int)(yp1 + ys1), GetColor(0, 0, 0), TRUE);
					float xp2, yp2;
					float xs2, ys2;

					xp2 = (float)y_r(160);
					yp2 = (float)DrawParts->disp_y - y_r(130);
					xs2 = (float)y_r(120) / 2;
					ys2 = (float)y_r(160) / 2;

					DrawBox((int)(xp2 - xs2), (int)(yp2 - ys2), (int)(xp2 + xs2), (int)(yp2 + ys2), GetColor(0, 0, 0), TRUE);

					float xp3, yp3;
					float xs3, ys3;

					xp3 = (float)y_r(270);
					yp3 = (float)DrawParts->disp_y - y_r(100);
					xs3 = (float)y_r(80) / 2;
					ys3 = (float)y_r(100) / 2;

					DrawBox((int)(xp3 - xs3), (int)(yp3 - ys3), (int)(xp3 + xs3), (int)(yp3 + ys3), GetColor(0, 0, 0), TRUE);
				}
				//レーダー
				{
					int xp1, yp1;
					int xs1, ys1;

					xs1 = y_r(256) / 2;
					ys1 = y_r(256) / 2;
					xp1 = DrawParts->disp_x - y_r(80) - xs1;
					yp1 = DrawParts->disp_y - y_r(300) - ys1;
					DrawBox((int)(xp1 - xs1), (int)(yp1 - ys1), (int)(xp1 + xs1), (int)(yp1 + ys1), Green, FALSE);
					DrawBox((int)(xp1 - xs1) + 1, (int)(yp1 - ys1) + 1, (int)(xp1 + xs1) - 1, (int)(yp1 + ys1) - 1, GetColor(0,0,0), FALSE);
					DrawBox((int)(xp1 - xs1) + 2, (int)(yp1 - ys1) + 2, (int)(xp1 + xs1) - 2, (int)(yp1 + ys1) - 2, Green, FALSE);
				}
				//情報
				{
					int xp1, yp1;
					int Num = 4;
					int ys1 = y_r(30);
					//名前、体力、魔力
					{
						xp1 = DrawParts->disp_x - y_r(300);
						yp1 = DrawParts->disp_y - y_r(30) - ys1 * Num - y_r(90);

						UI.Get(y_r(20)).Get_handle().DrawStringFormat(xp1 + y_r(50), yp1, Green, White, "%s", "Name");

						yp1 += y_r(25);
						UI.Get(y_r(20)).Get_handle().DrawString(xp1, yp1, "HP", Green, White);
						UI.Get(y_r(20)).Get_handle().DrawStringFormat(xp1 + y_r(50), yp1, Green, White, "%03d / %03d", 100, 100);

						yp1 += y_r(25);
						UI.Get(y_r(20)).Get_handle().DrawString(xp1, yp1, "MP", Green, White);
						UI.Get(y_r(20)).Get_handle().DrawStringFormat(xp1 + y_r(50), yp1, Green, White, "%03d / %03d", 100, 100);
					}
					//魔法、クールタイム
					{
						xp1 = DrawParts->disp_x - y_r(300);
						yp1 = DrawParts->disp_y - y_r(30) - ys1 * Num;
						for (int s = 0; s < Num; s++) {
							UI.Get(y_r(20)).Get_handle().DrawStringFormat(xp1, yp1, Green, White, "%s", "MAGIC");
							DrawBox(xp1 + y_r(100), yp1 + y_r(2), xp1 + y_r(100) + y_r(160), yp1 + y_r(2) + y_r(12), White, TRUE);
							DrawBox(xp1 + y_r(100) + 2, yp1 + y_r(2) + 2, xp1 + y_r(100) + y_r(160) - 2, yp1 + y_r(2) + y_r(12) - 2, GetColor(255, 255, 0), TRUE);
							yp1 += ys1;
						}
					}
				}
				//スピード
				{
					int xp1, yp1;
					xp1 = DrawParts->disp_x / 2 - y_r(300);
					yp1 = DrawParts->disp_y / 2 - y_r(10);
					UI.Get(y_r(20)).Get_handle().DrawStringFormat_RIGHT(xp1, yp1, Green, White, "%06.2f km/h", floatParam[1]);
				}
				//高度
				{
					int xp1, yp1;
					xp1 = DrawParts->disp_x / 2 + y_r(300);
					yp1 = DrawParts->disp_y / 2 - y_r(10);
					UI.Get(y_r(20)).Get_handle().DrawStringFormat(xp1, yp1, Green, White, "%07.2f m", floatParam[2]);
				}
			}

			void SetIntParam(int ID, int value) { intParam[ID] = value; }
			void SetfloatParam(int ID, float value) { floatParam[ID] = value; }
		};
	};
};