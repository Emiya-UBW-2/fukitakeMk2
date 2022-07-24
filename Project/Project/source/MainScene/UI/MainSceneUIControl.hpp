#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			GraphHandle HeartGraph;

			FontPool UI;

			int intParam[9]{ 0 };
			float floatParam[5]{ 0 };
			GraphHandle* ItemGraphPtr[3]{ 0 };

			int Blend3Int(int pInt1, int pInt2, int pInt3, float per) {
				int ans;
				ans = (int)(std::clamp<float>(per * 2.f - 1.f, 0.f, 1.f) * (float)pInt3);
				ans += (int)(std::clamp<float>((per < 0.5f) ? (per * 2.f) : ((1.f - per) * 2.f), 0.f, 1.f) * (float)pInt2);
				ans += (int)(std::clamp<float>((1.f - per) * 2.f - 1.f, 0.f, 1.f) * (float)pInt1);
				return ans;
			}

			COLOR_U8 Blend3Color(COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, float per) {
				int r, g, b;
				r = Blend3Int(Color1.r, Color2.r, Color3.r, per);
				g = Blend3Int(Color1.g, Color2.g, Color3.g, per);
				b = Blend3Int(Color1.b, Color2.b, Color3.b, per);
				return GetColorU8(r, g, b, 255);
			}
			void DrawGauge(int xp1, int yp1, int xp2, int yp2,
				int Param1, int ParamMax, int ParamBuf,
				COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, unsigned int ColorAdd, unsigned int ColorSub) {
				DrawBox(xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, GetColor(255, 255, 255), FALSE);
				DrawBox(xp1 + 1, yp1 + 1, xp2 - 1, yp2 - 1, GetColor(255, 255, 255), FALSE);
				int length = (xp2 - 2) - (xp1 + 2);

				COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, (float)Param1 / (float)ParamMax);

				DrawBox(xp1 + 2 + length * std::max(Param1, ParamBuf) / ParamMax, yp1 + 2, xp2 - 2, yp2 - 2, GetColor(0, 0, 0), TRUE);
				DrawBox(xp1 + 2, yp1 + 2, xp1 + 2 + length * Param1 / ParamMax, yp2 - 2, GetColor(Color.r, Color.g, Color.b), TRUE);
				DrawBox(xp1 + 2 + length * ParamBuf / ParamMax, yp1 + 2, xp1 + 2 + length * Param1 / ParamMax, yp2 - 2, (ParamBuf > Param1) ? ColorSub : ColorAdd, TRUE);
			}
		public:
			void Set(void) noexcept {
				HeartGraph = GraphHandle::Load("data/UI/Heart.png");
				for (int i = 0; i < 3; i++) {
					ItemGraphPtr[i] = nullptr;
				}
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
					float xp1_r, yp1_r;

					xp1 = (float)y_r(50);
					yp1 = (float)DrawParts->disp_y - y_r(100);
					xs1 = (float)y_r(80) / 2;
					ys1 = (float)y_r(100) / 2;

					float xp2, yp2;
					float xs2, ys2;
					float xp2_r, yp2_r;

					xp2 = (float)y_r(160);
					yp2 = (float)DrawParts->disp_y - y_r(130);
					xs2 = (float)y_r(120) / 2;
					ys2 = (float)y_r(160) / 2;

					float xp3, yp3;
					float xs3, ys3;
					float xp3_r, yp3_r;

					xp3 = (float)y_r(270);
					yp3 = (float)DrawParts->disp_y - y_r(100);
					xs3 = (float)y_r(80) / 2;
					ys3 = (float)y_r(100) / 2;

					float Lper = floatParam[3];//0->1にする
					float Rper = floatParam[4];//0->1にする
					//
					xp1_r = Leap(xp2, xp1, Lper);
					yp1_r = Leap(yp2, yp1, Lper);

					DrawBox((int)(xp1 - xs1), (int)(yp1 - ys1), (int)(xp1 + xs1), (int)(yp1 + ys1), GetColor(0, 0, 0), TRUE);
					if (ItemGraphPtr[0] != nullptr) {
						ItemGraphPtr[0]->DrawExtendGraph((int)(xp1_r - xs1), (int)(yp1_r - ys1), (int)(xp1_r + xs1), (int)(yp1_r + ys1), true);
					}
					//
					xp2_r = Leap(xp1, Leap(xp3, xp2, Lper), Rper);
					yp2_r = Leap(yp1, Leap(yp3, yp2, Lper), Rper);

					DrawBox((int)(xp2 - xs2), (int)(yp2 - ys2), (int)(xp2 + xs2), (int)(yp2 + ys2), GetColor(0, 0, 0), TRUE);
					if (ItemGraphPtr[1] != nullptr) {
						ItemGraphPtr[1]->DrawExtendGraph((int)(xp2_r - xs2), (int)(yp2_r - ys2), (int)(xp2_r + xs2), (int)(yp2_r + ys2), true);
					}
					//
					xp3_r = Leap(xp2, xp3, Rper);
					yp3_r = Leap(yp2, yp3, Rper);

					DrawBox((int)(xp3 - xs3), (int)(yp3 - ys3), (int)(xp3 + xs3), (int)(yp3 + ys3), GetColor(0, 0, 0), TRUE);
					if (ItemGraphPtr[2] != nullptr) {
						ItemGraphPtr[2]->DrawExtendGraph((int)(xp3_r - xs3), (int)(yp3_r - ys3), (int)(xp3_r + xs3), (int)(yp3_r + ys3), true);
					}
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
						yp1 = DrawParts->disp_y - y_r(30) - ys1 * Num - y_r(110);

						UI.Get(y_r(20)).Get_handle().DrawStringFormat(xp1 + y_r(50), yp1, Green, White, "%s", "Name");
						yp1 += y_r(25);

						UI.Get(y_r(20)).Get_handle().DrawString(xp1, yp1, "HP", Green, White);
						UI.Get(y_r(20)).Get_handle().DrawStringFormat(xp1 + y_r(50), yp1, Green, White, "%03d / %03d", intParam[3], intParam[4]);
						yp1 += y_r(25);

						DrawGauge(
							xp1, yp1, xp1 + y_r(200), yp1 + y_r(10),
							intParam[3], intParam[4], intParam[5], GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
							GetColor(0, 0, 255), GetColor(255, 0, 0)
							);
						yp1 += y_r(10);

						UI.Get(y_r(20)).Get_handle().DrawString(xp1, yp1, "MP", Green, White);
						UI.Get(y_r(20)).Get_handle().DrawStringFormat(xp1 + y_r(50), yp1, Green, White, "%03d / %03d", intParam[6], intParam[7]);
						yp1 += y_r(25);

						DrawGauge(
							xp1, yp1, xp1 + y_r(200), yp1 + y_r(10),
							intParam[6], intParam[7], intParam[8], GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
							GetColor(0, 0, 255), GetColor(255, 0, 0)
						);
						yp1 += y_r(10);
					}
					//魔法、クールタイム
					{
						xp1 = DrawParts->disp_x - y_r(300);
						yp1 = DrawParts->disp_y - y_r(30) - ys1 * Num;
						for (int s = 0; s < Num; s++) {
							UI.Get(y_r(20)).Get_handle().DrawStringFormat(xp1, yp1, Green, White, "%s", "MAGIC");

							DrawGauge(
								xp1 + y_r(100), yp1 + y_r(2), xp1 + y_r(100) + y_r(160), yp1 + y_r(2) + y_r(12),
								100, 100, 100, GetColorU8(255, 255, 0,255), GetColorU8(255, 255, 0, 255), GetColorU8(255, 255, 0, 255),
								GetColor(0, 0, 255), GetColor(255, 0, 0)
								);
							yp1 += ys1;
						}
					}
				}
				//スピード
				if (intParam[2] == 1) {
					int xp1, yp1;
					xp1 = DrawParts->disp_x / 2 - y_r(300);
					yp1 = DrawParts->disp_y / 2 - y_r(10);
					UI.Get(y_r(20)).Get_handle().DrawStringFormat_RIGHT(xp1, yp1, Green, White, "%06.2f km/h", floatParam[1]);
				}
				//高度
				if (intParam[2] == 1) {
					int xp1, yp1;
					xp1 = DrawParts->disp_x / 2 + y_r(300);
					yp1 = DrawParts->disp_y / 2 - y_r(10);
					UI.Get(y_r(20)).Get_handle().DrawStringFormat(xp1, yp1, Green, White, "%07.2f m", floatParam[2]);
				}
			}

			void SetIntParam(int ID, int value) { intParam[ID] = value; }
			void SetfloatParam(int ID, float value) { floatParam[ID] = value; }
			void SetItemGraph(int ID, const GraphHandle* value) { ItemGraphPtr[ID] = (GraphHandle*)value; }
		};
	};
};