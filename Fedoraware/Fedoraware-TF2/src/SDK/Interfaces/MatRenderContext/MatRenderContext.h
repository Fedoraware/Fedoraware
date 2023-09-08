#pragma once
#include "../../Includes/Includes.h"

struct Vector4D { float x, y, z, w; };
struct MorphWeight_t;
struct MaterialPrimitiveType_t;
struct MaterialNonInteractiveMode_t;
struct MaterialMatrixMode_t;
struct MaterialIndexFormat_t;
struct IMesh;
struct MaterialHeightClipMode_t;
enum MaterialFogMode_t
{
	MATERIAL_FOG_NONE,
	MATERIAL_FOG_LINEAR,
	MATERIAL_FOG_LINEAR_BELOW_FOG_Z,
};
struct MaterialCullMode_t;
struct LightDesc_t;
struct IVertexBuffer;
struct IMorph;
struct IIndexBuffer;
struct ICallQueue;
struct IAsyncTextureOperationReceiver;
struct FlashlightState_t;
struct DeformationBase_t;
struct ColorCorrectionHandle_t;

using Vector2D = Vec2;

using OcclusionQueryObjectHandle_t = void *;

class IMatRenderContext : public IRefCounted
{
public:
	virtual void				BeginRender() = 0;
	virtual void				EndRender() = 0;
	virtual void				Flush(bool flushHardware = false) = 0;
	virtual void				BindLocalCubemap(ITexture *pTexture) = 0;
	virtual void				SetRenderTarget(ITexture *pTexture) = 0;
	virtual ITexture *GetRenderTarget(void) = 0;
	virtual void				GetRenderTargetDimensions(int &width, int &height) const = 0;
	virtual void				Bind(IMaterial *material, void *proxyData = 0) = 0;
	virtual void				BindLightmapPage(int lightmapPageID) = 0;
	virtual void				DepthRange(float zNear, float zFar) = 0;
	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) = 0;
	virtual void				ReadPixels(int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat) = 0;
	virtual void				SetAmbientLight(float r, float g, float b) = 0;
	virtual void				SetLight(int lightNum, const LightDesc_t &desc) = 0;
	virtual void				SetAmbientLightCube(Vector4D cube[6]) = 0;
	virtual void				CopyRenderTargetToTexture(ITexture *pTexture) = 0;
	virtual void				SetFrameBufferCopyTexture(ITexture *pTexture, int textureIndex = 0) = 0;
	virtual ITexture *GetFrameBufferCopyTexture(int textureIndex) = 0;
	virtual void				MatrixMode(MaterialMatrixMode_t matrixMode) = 0;
	virtual void				PushMatrix(void) = 0;
	virtual void				PopMatrix(void) = 0;
	virtual void				LoadMatrix(VMatrix const &matrix) = 0;
	virtual void				LoadMatrix(matrix3x4 const &matrix) = 0;
	virtual void				MultMatrix(VMatrix const &matrix) = 0;
	virtual void				MultMatrix(matrix3x4 const &matrix) = 0;
	virtual void				MultMatrixLocal(VMatrix const &matrix) = 0;
	virtual void				MultMatrixLocal(matrix3x4 const &matrix) = 0;
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, VMatrix *matrix) = 0;
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, matrix3x4 *matrix) = 0;
	virtual void				LoadIdentity(void) = 0;
	virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) = 0;
	virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) = 0;
	virtual void				PickMatrix(int x, int y, int width, int height) = 0;
	virtual void				Rotate(float angle, float x, float y, float z) = 0;
	virtual void				Translate(float x, float y, float z) = 0;
	virtual void				Scale(float x, float y, float z) = 0;
	virtual void				Viewport(int x, int y, int width, int height) = 0;
	virtual void				GetViewport(int &x, int &y, int &width, int &height) const = 0;
	virtual void				CullMode(MaterialCullMode_t cullMode) = 0;
	virtual void				SetHeightClipMode(MaterialHeightClipMode_t nHeightClipMode) = 0;
	virtual void				SetHeightClipZ(float z) = 0;
	virtual void				FogMode(MaterialFogMode_t fogMode) = 0;
	virtual void				FogStart(float fStart) = 0;
	virtual void				FogEnd(float fEnd) = 0;
	virtual void				SetFogZ(float fogZ) = 0;
	virtual MaterialFogMode_t	GetFogMode(void) = 0;
	virtual void				FogColor3f(float r, float g, float b) = 0;
	virtual void				FogColor3fv(float const *rgb) = 0;
	virtual void				FogColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void				FogColor3ubv(unsigned char const *rgb) = 0;
	virtual void				GetFogColor(unsigned char *rgb) = 0;
	virtual void				SetNumBoneWeights(int numBones) = 0;
	virtual IMesh *CreateStaticMesh(VertexFormat_t fmt, const char *pTextureBudgetGroup, IMaterial *pMaterial = NULL) = 0;
	virtual void DestroyStaticMesh(IMesh *mesh) = 0;
	virtual IMesh *GetDynamicMesh(
		bool buffered = true,
		IMesh *pVertexOverride = 0,
		IMesh *pIndexOverride = 0,
		IMaterial *pAutoBind = 0) = 0;
	virtual IVertexBuffer *CreateStaticVertexBuffer(VertexFormat_t fmt, int nVertexCount, const char *pTextureBudgetGroup) = 0;
	virtual IIndexBuffer *CreateStaticIndexBuffer(MaterialIndexFormat_t fmt, int nIndexCount, const char *pTextureBudgetGroup) = 0;
	virtual void DestroyVertexBuffer(IVertexBuffer *) = 0;
	virtual void DestroyIndexBuffer(IIndexBuffer *) = 0;
	virtual IVertexBuffer *GetDynamicVertexBuffer(int streamID, VertexFormat_t vertexFormat, bool bBuffered = true) = 0;
	virtual IIndexBuffer *GetDynamicIndexBuffer(MaterialIndexFormat_t fmt, bool bBuffered = true) = 0;
	virtual void BindVertexBuffer(int streamID, IVertexBuffer *pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions = 1) = 0;
	virtual void BindIndexBuffer(IIndexBuffer *pIndexBuffer, int nOffsetInBytes) = 0;
	virtual void Draw(MaterialPrimitiveType_t primitiveType, int firstIndex, int numIndices) = 0;
	virtual int  SelectionMode(bool selectionMode) = 0;
	virtual void SelectionBuffer(unsigned int *pBuffer, int size) = 0;
	virtual void ClearSelectionNames() = 0;
	virtual void LoadSelectionName(int name) = 0;
	virtual void PushSelectionName(int name) = 0;
	virtual void PopSelectionName() = 0;
	virtual void		ClearColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void		ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) = 0;
	virtual void	OverrideDepthEnable(bool bEnable, bool bDepthEnable) = 0;
	virtual void	DrawScreenSpaceQuad(IMaterial *pMaterial) = 0;
	virtual void	SyncToken(const char *pToken) = 0;
	virtual float	ComputePixelWidthOfSphere(const Vector &origin, float flRadius) = 0;
	virtual OcclusionQueryObjectHandle_t CreateOcclusionQueryObject(void) = 0;
	virtual void DestroyOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;
	virtual void BeginOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;
	virtual void EndOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;
	virtual int OcclusionQuery_GetNumPixelsRendered(OcclusionQueryObjectHandle_t) = 0;
	virtual void SetFlashlightMode(bool bEnable) = 0;
	virtual void SetFlashlightState(const FlashlightState_t &state, const VMatrix &worldToTexture) = 0;
	virtual MaterialHeightClipMode_t GetHeightClipMode() = 0;
	virtual float	ComputePixelDiameterOfSphere(const Vector &vecAbsOrigin, float flRadius) = 0;
	virtual void	EnableUserClipTransformOverride(bool bEnable) = 0;
	virtual void	UserClipTransform(const VMatrix &worldToView) = 0;
	virtual bool GetFlashlightMode() const = 0;
	virtual void ResetOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;
	virtual void Unused3() {}
	virtual IMorph *CreateMorph(MorphFormat_t format, const char *pDebugName) = 0;
	virtual void DestroyMorph(IMorph *pMorph) = 0;
	virtual void BindMorph(IMorph *pMorph) = 0;
	virtual void SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t *pWeights) = 0;
	virtual void Unused4() {};
	virtual void Unused5() {};
	virtual void Unused6() {};
	virtual void Unused7() {};
	virtual void Unused8() {};
	virtual void ReadPixelsAndStretch(Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *pBuffer, ImageFormat dstFormat, int nDstStride) = 0;
	virtual void GetWindowSize(int &width, int &height) const = 0;
	virtual void DrawScreenSpaceRectangle(
		IMaterial *pMaterial,
		int destx, int desty,
		int width, int height,
		float src_texture_x0, float src_texture_y0,
		float src_texture_x1, float src_texture_y1,
		int src_texture_width, int src_texture_height,
		void *pClientRenderable = NULL,
		int nXDice = 1,
		int nYDice = 1) = 0;
	virtual void LoadBoneMatrix(int boneIndex, const matrix3x4 &matrix) = 0;
	virtual void PushRenderTargetAndViewport() = 0;
	virtual void PushRenderTargetAndViewport(ITexture *pTexture) = 0;
	virtual void PushRenderTargetAndViewport(ITexture *pTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;
	virtual void PushRenderTargetAndViewport(ITexture *pTexture, ITexture *pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;
	virtual void PopRenderTargetAndViewport(void) = 0;
	virtual void BindLightmapTexture(ITexture *pLightmapTexture) = 0;
	virtual void CopyRenderTargetToTextureEx(ITexture *pTexture, int nRenderTargetID, Rect_t *pSrcRect, Rect_t *pDstRect = NULL) = 0;
	virtual void CopyTextureToRenderTargetEx(int nRenderTargetID, ITexture *pTexture, Rect_t *pSrcRect, Rect_t *pDstRect = NULL) = 0;
	virtual void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) = 0;
	virtual void SetFloatRenderingParameter(int parm_number, float value) = 0;
	virtual void SetIntRenderingParameter(int parm_number, int value) = 0;
	virtual void SetVectorRenderingParameter(int parm_number, Vector const &value) = 0;
	virtual void SetStencilEnable(bool onoff) = 0;
	virtual void SetStencilFailOperation(StencilOperation_t op) = 0;
	virtual void SetStencilZFailOperation(StencilOperation_t op) = 0;
	virtual void SetStencilPassOperation(StencilOperation_t op) = 0;
	virtual void SetStencilCompareFunction(StencilComparisonFunction_t cmpfn) = 0;
	virtual void SetStencilReferenceValue(int ref) = 0;
	virtual void SetStencilTestMask(uint32_t msk) = 0;
	virtual void SetStencilWriteMask(uint32_t msk) = 0;
	virtual void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value) = 0;
	virtual void SetRenderTargetEx(int nRenderTargetID, ITexture *pTexture) = 0;
	virtual void PushCustomClipPlane(const float *pPlane) = 0;
	virtual void PopCustomClipPlane(void) = 0;
	virtual void GetMaxToRender(IMesh *pMesh, bool bMaxUntilFlush, int *pMaxVerts, int *pMaxIndices) = 0;
	virtual int GetMaxVerticesToRender(IMaterial *pMaterial) = 0;
	virtual int GetMaxIndicesToRender() = 0;
	virtual void DisableAllLocalLights() = 0;
	virtual int CompareMaterialCombos(IMaterial *pMaterial1, IMaterial *pMaterial2, int lightMapID1, int lightMapID2) = 0;
	virtual IMesh *GetFlexMesh() = 0;
	virtual void SetFlashlightStateEx(const FlashlightState_t &state, const VMatrix &worldToTexture, ITexture *pFlashlightDepthTexture) = 0;
	virtual ITexture *GetLocalCubemap() = 0;
	virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) = 0;
	virtual bool EnableClipping(bool bEnable) = 0;
	virtual void GetFogDistances(float *fStart, float *fEnd, float *fFogZ) = 0;
	virtual void BeginPIXEvent(unsigned long color, const char *szName) = 0;
	virtual void EndPIXEvent() = 0;
	virtual void SetPIXMarker(unsigned long color, const char *szName) = 0;
	virtual void BeginBatch(IMesh *pIndices) = 0;
	virtual void BindBatch(IMesh *pVertices, IMaterial *pAutoBind = NULL) = 0;
	virtual void DrawBatch(int firstIndex, int numIndices) = 0;
	virtual void EndBatch() = 0;
	virtual ICallQueue *GetCallQueue() = 0;
	virtual void GetWorldSpaceCameraPosition(Vector *pCameraPos) = 0;
	virtual void GetWorldSpaceCameraVectors(Vector *pVecForward, Vector *pVecRight, Vector *pVecUp) = 0;
	virtual void				ResetToneMappingScale(float monoscale) = 0;
	virtual void				SetGoalToneMappingScale(float monoscale) = 0;
	virtual void				TurnOnToneMapping() = 0;
	virtual void				SetToneMappingScaleLinear(const Vector &scale) = 0;
	virtual Vector				GetToneMappingScaleLinear(void) = 0;
	virtual void				SetShadowDepthBiasFactors(float fSlopeScaleDepthBias, float fDepthBias) = 0;
	virtual void				PerformFullScreenStencilOperation(void) = 0;
	virtual void				SetLightingOrigin(Vector vLightingOrigin) = 0;
	virtual void				SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor) = 0;
	virtual void				BeginMorphAccumulation() = 0;
	virtual void				EndMorphAccumulation() = 0;
	virtual void				AccumulateMorph(IMorph *pMorph, int nMorphCount, const MorphWeight_t *pWeights) = 0;
	virtual void				PushDeformation(DeformationBase_t const *Deformation) = 0;
	virtual void				PopDeformation() = 0;
	virtual int					GetNumActiveDeformations() const = 0;
	virtual bool				GetMorphAccumulatorTexCoord(Vector2D *pTexCoord, IMorph *pMorph, int nVertex) = 0;
	virtual IMesh *GetDynamicMeshEx(VertexFormat_t vertexFormat, bool bBuffered = true,
		IMesh *pVertexOverride = 0, IMesh *pIndexOverride = 0, IMaterial *pAutoBind = 0) = 0;

	virtual void				FogMaxDensity(float flMaxDensity) = 0;
	virtual IMaterial *GetCurrentMaterial() = 0;
	virtual int  GetCurrentNumBones() const = 0;
	virtual void *GetCurrentProxy() = 0;
	virtual void EnableColorCorrection(bool bEnable) = 0;
	virtual ColorCorrectionHandle_t AddLookup(const char *pName) = 0;
	virtual bool RemoveLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LoadLookup(ColorCorrectionHandle_t handle, const char *pLookupName) = 0;
	virtual void UnlockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void SetLookupWeight(ColorCorrectionHandle_t handle, float flWeight) = 0;
	virtual void ResetLookupWeights() = 0;
	virtual void SetResetable(ColorCorrectionHandle_t handle, bool bResetable) = 0;
	virtual void SetFullScreenDepthTextureValidityFlag(bool bIsValid) = 0;
	virtual void SetNonInteractivePacifierTexture(ITexture *pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) = 0;
	virtual void SetNonInteractiveTempFullscreenBuffer(ITexture *pTexture, MaterialNonInteractiveMode_t mode) = 0;
	virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode) = 0;
	virtual void RefreshFrontBufferNonInteractive() = 0;
	virtual void *LockRenderData(int nSizeInBytes) = 0;
	virtual void			UnlockRenderData(void *pData) = 0;
	virtual void			AddRefRenderData() = 0;
	virtual void			ReleaseRenderData() = 0;
	virtual bool			IsRenderData(const void *pData) const = 0;
	virtual void			PrintfVA(char *fmt, va_list vargs) = 0;
	virtual void			Printf(const char *fmt, ...) = 0;
	virtual float			Knob(char *knobname, float *setvalue = NULL) = 0;
	virtual void OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable) = 0;
	virtual void OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable) = 0;
	virtual void ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth) = 0;
	virtual void AsyncCreateTextureFromRenderTarget(ITexture *pSrcRt, const char *pDstName, ImageFormat dstFmt, bool bGenMips, int nAdditionalCreationFlags, IAsyncTextureOperationReceiver *pRecipient, void *pExtraArgs) = 0;
};