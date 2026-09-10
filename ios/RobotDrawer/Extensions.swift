import Foundation

extension CGPoint {
    func distance(to other: CGPoint) -> CGFloat {
        let dx = other.x - self.x
        let dy = other.y - self.y
        return sqrt(dx * dx + dy * dy)
    }
}

extension LinearGradient {
    init(
        gradient: Gradient,
        startPoint: UnitPoint,
        endPoint: UnitPoint
    ) {
        self.init(
            gradient: gradient,
            startPoint: startPoint,
            endPoint: endPoint
        )
    }
}

extension UnitPoint {
    static let topLeadingAngle = UnitPoint(x: 0, y: 0)
    static let bottomTrailingAngle = UnitPoint(x: 1, y: 1)
}
